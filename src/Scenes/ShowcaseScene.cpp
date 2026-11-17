// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/ShowcaseScene.h"
#include "Core/Resources.h"
#include "Lighting/Material.h"
#include "Rendering/DrawableObject.h"
#include "Rendering/ShaderProgram.h"
#include "Transformations/CustomMatrix.h"
#include "Transformations/DynamicRotate.h"
#include "Transformations/GrowScale.h"
#include "Transformations/LineMove.h"
#include "Transformations/Oscillate.h"
#include "Transformations/Transformation.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

class ShearWobble : public Transformation {
public:
    glm::mat4 getMatrix() const override {
        glm::mat4 matrix(1.0f);
        matrix[1][0] = amount;
        matrix[1][2] = 0.4f * amount;
        return matrix;
    }

    float amount = 0.0f;
    float velocity = 0.0f;
};

static const float AMBIENT = 0.12f;
static const float TWO_PI = glm::two_pi<float>();
static const glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);

static glm::vec3 hexColor(unsigned rgb) {
    return glm::vec3((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF) / 255.0f;
}

static glm::vec3 hue(float h, float saturation = 0.75f, float value = 1.0f) {
    h -= std::floor(h);
    const glm::vec3 k = glm::abs(glm::fract(glm::vec3(h) + glm::vec3(1.0f, 2.0f / 3.0f, 1.0f / 3.0f)) * 6.0f - 3.0f);
    return value * glm::mix(glm::vec3(1.0f), glm::clamp(k - 1.0f, 0.0f, 1.0f), saturation);
}

static Material glowMaterial() {
    Material material;
    material.ambient = glm::vec3(1.0f / AMBIENT);
    material.diffuse = glm::vec3(0.0f);
    material.specular = glm::vec3(0.0f);
    material.shininess = 1.0f;
    return material;
}

static Material blackMaterial() {
    Material material;
    material.ambient = glm::vec3(0.0f);
    material.diffuse = glm::vec3(0.0f);
    material.specular = glm::vec3(0.0f);
    material.shininess = 1.0f;
    return material;
}

static Material stoneMaterial() {
    Material material;
    material.ambient = glm::vec3(0.6f);
    material.diffuse = glm::vec3(0.75f);
    material.specular = glm::vec3(0.15f);
    material.shininess = 8.0f;
    return material;
}

static Material glossyMaterial() {
    Material material;
    material.ambient = glm::vec3(0.4f);
    material.diffuse = glm::vec3(0.85f);
    material.specular = glm::vec3(0.9f);
    material.shininess = 96.0f;
    return material;
}

static std::vector<Material> materialPalette() {
    return {
        Material::gold(), Material::silver(), Material::copper(), Material::bronze(), Material::chrome(),
        Material::pearl(), Material::emerald(), Material::ruby(), Material::plastic(), Material::rubber(),
    };
}

static void aimCamera(Camera* camera, const glm::vec3& eye, const glm::vec3& target) {
    const glm::vec3 direction = glm::normalize(target - eye);
    camera->setPosition(eye, glm::degrees(std::acos(glm::clamp(direction.y, -1.0f, 1.0f))), glm::degrees(std::atan2(direction.z, direction.x)));
}

static float smooth01(float x) {
    x = glm::clamp(x, 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

const glm::vec3 ShowcaseScene::SUN_POSITION = glm::vec3(0.0f, 3.4f, 0.0f);
static const float RING_RADIUS = 5.0f;
static const float RING_CUBE_HEIGHT = 1.75f;
static const glm::vec3 HELIX_CENTER = glm::vec3(-8.0f, 0.0f, -1.0f);
static const glm::vec3 EQUALIZER_CENTER = glm::vec3(8.0f, 0.0f, -1.0f);
static const glm::vec3 GATE_POSITION = glm::vec3(0.0f, 0.0f, 9.4f);
static const float PENDULUM_Z = -8.5f;
static const float PENDULUM_PIVOT_Y = 4.6f;

static const int MAX_SPAWNED = 16;
static const int COLLECTIBLE_COUNT = 8;
static const int TELEMETRY_LEDS = 8;
static const int SPEED_LEDS = 6;
static const float SPAWN_HEIGHT = 7.0f;
static const float GRAVITY = 18.0f;
static const float BOUNCE = 0.55f;
static const float DIE_DURATION = 0.35f;
static const float FLASH_DURATION = 0.6f;
static const float MINIATURE_ZOOM = 0.3f;
static const float MINIATURE_LIFT = 2.6f;
static const float MINIATURE_SPIN = 12.0f;

static const glm::vec3 ENVIRONMENT_COLORS[3] = { hexColor(0x3B5BDB), hexColor(0x9B5DE5), hexColor(0x9AA5B1) };

GrowScale* ShowcaseScene::intro(float durationSeconds, const glm::vec3& from, const glm::vec3& pivot) {
    GrowScale* animation = make<GrowScale>(getClock(), from, glm::vec3(1.0f), durationSeconds, pivot);
    introAnimations.push_back(animation);
    return animation;
}

DrawableObject* ShowcaseScene::spawn(Transformation* transformation, const Material& material, const glm::vec3& color, bool textured, bool decorative) {
    DrawableObject* object = decorative ? createStaticObject(cubeModel, litShader, transformation) : createObject(cubeModel, litShader, transformation);
    object->setMaterial(material);
    object->setColor(color);

    if (textured) {
        object->setTexture(defaultTexture);
    }

    return object;
}

DrawableObject* ShowcaseScene::hudSquare(HudAnchor anchor, const glm::vec2& offset, float scale) {
    return createHudObject(cubeModel, anchor, offset, scale);
}

glm::vec3 ShowcaseScene::toWorld(const glm::vec3& local) {
    return glm::vec3(getBase()->getMatrix() * glm::vec4(local, 1.0f));
}

void ShowcaseScene::create() {
    cubeModel = resources.getModel("cube");
    litShader = resources.getShader("lit.vert", "showcase.frag");
    defaultTexture = resources.getTexture(Texture::DEFAULT_PATH);

    TextureOptions crisp;
    crisp.repeat = false;
    crisp.mipmaps = false;
    crispTexture = resources.getTexture(Texture::DEFAULT_PATH, crisp);

    setAmbientLight(glm::vec3(AMBIENT));

    aimCamera(getCamera(), glm::vec3(0.0f, 4.0f, 15.5f), glm::vec3(0.0f, 2.0f, 0.0f));
    getCamera()->setSpeed(3.0f);

    if (DrawableObject* ground = getGround()) {
        ground->setColor(glm::vec3(0.55f));
        ground->setProgram(litShader);
    }

    buildSolarSystem();
    buildMaterialRing();
    buildPendulumWave();
    buildHelix();
    buildEqualizer();
    buildDrones();
    buildLights();
    buildGate();
    buildSky();
    buildHud();

    applyEnvironment();
}

void ShowcaseScene::buildSolarSystem() {
    SceneClock& clock = getClock();

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(glm::vec3(0.0f, 0.6f, 0.0f)));
    t->addStatic(Scale(glm::vec3(1.6f, 1.2f, 1.6f)));
    spawn(t, stoneMaterial(), glm::vec3(1.0f), true);

    t = makeComposite();
    t->addStatic(Translate(glm::vec3(0.0f, 1.25f, 0.0f)));
    t->addStatic(Scale(glm::vec3(1.8f, 0.1f, 1.8f)));
    spawn(t, Material::gold());

    sunPulse = make<Scale>(1.0f);
    sunBurst = make<GrowScale>(clock, glm::vec3(1.0f), glm::vec3(1.0f), 0.01f);
    GrowScale* sunGrow = intro(1.6f);

    t = makeComposite();
    t->addStatic(Translate(SUN_POSITION));
    t->addDynamic(sunGrow);
    t->addDynamic(sunBurst);
    t->addDynamic(sunPulse);
    t->addDynamic(make<DynamicRotate>(clock, 25.0f, UP));
    t->addStatic(Scale(0.85f));
    sunCubes.push_back(spawn(t, glowMaterial(), hexColor(0xFFC94A)));

    t = makeComposite();
    t->addStatic(Translate(SUN_POSITION));
    t->addDynamic(sunGrow);
    t->addDynamic(sunBurst);
    t->addDynamic(sunPulse);
    t->addDynamic(make<DynamicRotate>(clock, -40.0f, glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
    t->addStatic(Rotate(45.0f, glm::vec3(1.0f, 0.0f, 1.0f)));
    t->addStatic(Scale(0.85f));
    sunCubes.push_back(spawn(t, glowMaterial(), hexColor(0xFF7A2E)));

    sunLight = addLight();
    sunLight->setPosition(SUN_POSITION);
    sunLight->setColor(hexColor(0xFFC37A));
    sunLight->setIntensity(3.0f);
    sunLight->setAttenuation(1.0f, 0.09f, 0.032f);

    struct Planet {
        float radius;
        float size;
        float speed;
        float tiltDeg;
        float startDeg;
        Material material;
        bool hasMoon;
    };

    const std::vector<Planet> planets = {
        { 1.3f, 0.22f, 80.0f, 6.0f, 0.0f, Material::ruby(), false },
        { 1.95f, 0.34f, 48.0f, -4.0f, 140.0f, Material::emerald(), true },
        { 2.7f, 0.28f, 30.0f, 10.0f, 260.0f, Material::copper(), true },
    };

    for (std::size_t i = 0; i < planets.size(); ++i) {
        const Planet& planet = planets[i];
        DynamicRotate* orbit = make<DynamicRotate>(clock, planet.speed, UP, planet.startDeg);
        DynamicRotate* spin = make<DynamicRotate>(clock, 120.0f, glm::normalize(glm::vec3(0.3f, 1.0f, 0.0f)));
        GrowScale* grow = intro(1.5f + 0.4f * static_cast<float>(i));

        t = makeComposite();
        t->addStatic(Translate(SUN_POSITION));
        t->addStatic(Rotate(planet.tiltDeg, glm::vec3(0.0f, 0.0f, 1.0f)));
        t->addDynamic(orbit);
        t->addStatic(Translate(glm::vec3(planet.radius, 0.0f, 0.0f)));
        t->addDynamic(grow);
        t->addDynamic(spin);
        t->addStatic(Scale(planet.size));
        spawn(t, planet.material);

        if (planet.hasMoon) {
            t = makeComposite();
            t->addStatic(Translate(SUN_POSITION));
            t->addStatic(Rotate(planet.tiltDeg, glm::vec3(0.0f, 0.0f, 1.0f)));
            t->addDynamic(orbit);
            t->addStatic(Translate(glm::vec3(planet.radius, 0.0f, 0.0f)));
            t->addDynamic(grow);
            t->addDynamic(make<DynamicRotate>(clock, 200.0f, UP));
            t->addStatic(Translate(glm::vec3(planet.size + 0.25f, 0.0f, 0.0f)));
            t->addStatic(Scale(0.09f));
            spawn(t, Material::silver());
        }

        const int dots = 40;
        for (int d = 0; d < dots; ++d) {
            const float angle = TWO_PI * static_cast<float>(d) / static_cast<float>(dots);
            t = makeComposite();
            t->addStatic(Translate(SUN_POSITION));
            t->addStatic(Rotate(planet.tiltDeg, glm::vec3(0.0f, 0.0f, 1.0f)));
            t->addStatic(Translate(glm::vec3(std::cos(angle), 0.0f, std::sin(angle)) * planet.radius));
            t->addStatic(Scale(0.025f));
            spawn(t, glowMaterial(), hexColor(0xFFC94A) * 0.55f, false, true);
        }
    }
}

void ShowcaseScene::buildMaterialRing() {
    SceneClock& clock = getClock();

    const std::vector<Material> materials = materialPalette();
    const int count = static_cast<int>(materials.size());

    for (int i = 0; i < count; ++i) {
        const float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(count) + glm::half_pi<float>();
        const glm::vec3 base(RING_RADIUS * std::cos(angle), 0.0f, RING_RADIUS * std::sin(angle));
        const glm::vec3 color = hue(static_cast<float>(i) / static_cast<float>(count), 0.65f);

        ringColors.push_back(color);
        ringPositions.push_back(base + glm::vec3(0.0f, RING_CUBE_HEIGHT, 0.0f));

        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(base + glm::vec3(0.0f, 0.5f, 0.0f)));
        t->addStatic(Rotate(-glm::degrees(angle), UP));
        t->addStatic(Scale(glm::vec3(0.9f, 1.0f, 0.9f)));
        spawn(t, stoneMaterial(), glm::vec3(1.0f), true);

        t = makeComposite();
        t->addStatic(Translate(base + glm::vec3(0.0f, 1.02f, 0.0f)));
        t->addStatic(Rotate(-glm::degrees(angle), UP));
        t->addStatic(Scale(glm::vec3(1.0f, 0.04f, 1.0f)));
        ringTrims.push_back(spawn(t, glowMaterial(), color * 0.4f, false, true));

        Translate* bob = make<Translate>(glm::vec3(0.0f));
        bobbers.push_back({ bob, static_cast<float>(i) * 0.7f });

        const float spinSpeed = 40.0f + 6.0f * static_cast<float>(i);
        DynamicRotate* spin = make<DynamicRotate>(clock, spinSpeed, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
        ringSpins.push_back(spin);
        ringSpinSpeeds.push_back(spinSpeed);
        ringBoosts.push_back(0.0f);

        t = makeComposite();
        t->addStatic(Translate(ringPositions.back()));
        t->addDynamic(bob);
        t->addDynamic(intro(0.8f + 0.18f * static_cast<float>(i)));
        t->addDynamic(spin);
        t->addStatic(Scale(0.7f));
        ringCubes.push_back(spawn(t, materials[i]));

        Light* light = addLight();
        light->setPosition(base + glm::vec3(0.0f, RING_CUBE_HEIGHT + 1.2f, 0.0f));
        light->setColor(color);
        light->setAttenuation(1.0f, 0.35f, 0.44f);
        light->setIntensity(2.5f);
        ringLights.push_back(light);
    }
}

void ShowcaseScene::buildPendulumWave() {
    SceneClock& clock = getClock();

    const int count = 15;
    const float spacing = 0.8f;
    const float halfWidth = 0.5f * spacing * static_cast<float>(count - 1);
    const float gravity = 9.81f;
    const float lengthScale = 5.0f;

    for (float side : { -1.0f, 1.0f }) {
        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(glm::vec3(side * (halfWidth + 0.7f), 2.4f, PENDULUM_Z)));
        t->addStatic(Scale(glm::vec3(0.25f, 4.8f, 0.25f)));
        spawn(t, stoneMaterial(), glm::vec3(1.0f), true);
    }

    TransformationComposite* beam = makeComposite();
    beam->addStatic(Translate(glm::vec3(0.0f, PENDULUM_PIVOT_Y + 0.15f, PENDULUM_Z)));
    beam->addStatic(Scale(glm::vec3(2.0f * halfWidth + 1.65f, 0.25f, 0.25f)));
    spawn(beam, Material::chrome());

    for (int i = 0; i < count; ++i) {
        const float frequency = static_cast<float>(40 + i) / 60.0f;
        const float omega = TWO_PI * frequency;
        const float length = lengthScale * gravity / (omega * omega);
        const glm::vec3 pivot(-halfWidth + spacing * static_cast<float>(i), PENDULUM_PIVOT_Y, PENDULUM_Z);

        Oscillate* swing = make<Oscillate>(clock, 28.0f, frequency, glm::vec3(1.0f, 0.0f, 0.0f));
        pendulums.push_back({ swing, frequency });

        TransformationComposite* rod = makeComposite();
        rod->addStatic(Translate(pivot));
        rod->addDynamic(swing);
        rod->addStatic(Translate(glm::vec3(0.0f, -0.5f * length, 0.0f)));
        rod->addStatic(Scale(glm::vec3(0.03f, length, 0.03f)));
        spawn(rod, Material::silver(), glm::vec3(1.0f), false, true);

        TransformationComposite* bob = makeComposite();
        bob->addStatic(Translate(pivot));
        bob->addDynamic(swing);
        bob->addStatic(Translate(glm::vec3(0.0f, -length, 0.0f)));
        bob->addStatic(Rotate(45.0f, UP));
        bob->addStatic(Scale(0.32f));
        spawn(bob, glossyMaterial(), hue(static_cast<float>(i) / static_cast<float>(count), 0.8f));
    }

    Light* light = addLight();
    light->setPosition(glm::vec3(0.0f, 3.0f, PENDULUM_Z + 2.5f));
    light->setColor(hexColor(0xBFD8FF));
    light->setIntensity(2.0f);
    light->setAttenuation(1.0f, 0.09f, 0.032f);
}

void ShowcaseScene::buildHelix() {
    SceneClock& clock = getClock();

    const int levels = 18;
    const float radius = 0.85f;
    const float stepY = 0.32f;
    const float startY = 0.7f;
    const float stepDeg = 34.0f;

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(HELIX_CENTER + glm::vec3(0.0f, 0.1f, 0.0f)));
    t->addStatic(Scale(glm::vec3(2.6f, 0.2f, 2.6f)));
    spawn(t, stoneMaterial(), glm::vec3(1.0f), true);

    t = makeComposite();
    t->addStatic(Translate(HELIX_CENTER + glm::vec3(0.0f, 0.21f, 0.0f)));
    t->addStatic(Scale(glm::vec3(2.7f, 0.03f, 2.7f)));
    spawn(t, glowMaterial(), hexColor(0xC86BFA) * 0.5f, false, true);

    GrowScale* grow = intro(3.0f, glm::vec3(1.0f, 0.001f, 1.0f));
    DynamicRotate* spin = make<DynamicRotate>(clock, 35.0f, UP);

    for (int k = 0; k < levels; ++k) {
        const float angle = glm::radians(stepDeg * static_cast<float>(k));
        const float y = startY + stepY * static_cast<float>(k);
        const glm::vec3 offset(radius * std::cos(angle), 0.0f, radius * std::sin(angle));

        for (float side : { 1.0f, -1.0f }) {
            t = makeComposite();
            t->addStatic(Translate(HELIX_CENTER));
            t->addDynamic(grow);
            t->addDynamic(spin);
            t->addStatic(Translate(side * offset + glm::vec3(0.0f, y, 0.0f)));
            t->addStatic(Rotate(-glm::degrees(angle), UP));
            t->addStatic(Scale(0.22f));
            spawn(t, side > 0.0f ? Material::ruby() : Material::emerald());
        }

        t = makeComposite();
        t->addStatic(Translate(HELIX_CENTER));
        t->addDynamic(grow);
        t->addDynamic(spin);
        t->addStatic(Translate(glm::vec3(0.0f, y, 0.0f)));
        t->addStatic(Rotate(-glm::degrees(angle), UP));
        t->addStatic(Scale(glm::vec3(2.0f * radius, 0.05f, 0.05f)));
        spawn(t, Material::pearl(), glm::vec3(1.0f), false, true);
    }

    Light* light = addLight();
    light->setPosition(HELIX_CENTER + glm::vec3(1.8f, 4.0f, 2.0f));
    light->setColor(hexColor(0xC86BFA));
    light->setIntensity(2.5f);
    light->setAttenuation(1.0f, 0.14f, 0.07f);
}

void ShowcaseScene::buildEqualizer() {
    const int size = 7;
    const float spacing = 0.5f;
    const float half = 0.5f * spacing * static_cast<float>(size - 1);

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(EQUALIZER_CENTER + glm::vec3(0.0f, 0.05f, 0.0f)));
    t->addStatic(Scale(glm::vec3(2.0f * half + 0.7f, 0.1f, 2.0f * half + 0.7f)));
    spawn(t, stoneMaterial(), glm::vec3(1.0f), true);

    for (int ix = 0; ix < size; ++ix) {
        for (int iz = 0; iz < size; ++iz) {
            const float x = -half + spacing * static_cast<float>(ix);
            const float z = -half + spacing * static_cast<float>(iz);

            Scale* height = make<Scale>(glm::vec3(1.0f));

            t = makeComposite();
            t->addStatic(Translate(EQUALIZER_CENTER + glm::vec3(x, 0.1f, z)));
            t->addDynamic(height);
            t->addStatic(Translate(glm::vec3(0.0f, 0.5f, 0.0f)));
            t->addStatic(Scale(glm::vec3(0.42f, 1.0f, 0.42f)));

            DrawableObject* column = spawn(t, glossyMaterial());
            columns.push_back({ column, height, std::sqrt(x * x + z * z), x });
        }
    }

    equalizerLight = addLight();
    equalizerLight->setPosition(EQUALIZER_CENTER + glm::vec3(-1.0f, 3.5f, 2.5f));
    equalizerLight->setColor(glm::vec3(1.0f));
    equalizerLight->setIntensity(2.0f);
    equalizerLight->setAttenuation(1.0f, 0.14f, 0.07f);
}

void ShowcaseScene::buildDrones() {
    SceneClock& clock = getClock();

    struct Route {
        glm::vec3 from;
        glm::vec3 to;
        float speed;
        glm::vec3 color;
    };

    const std::vector<Route> routes = {
        { glm::vec3(-9.0f, 2.6f, 7.5f), glm::vec3(9.0f, 2.6f, 7.5f), 2.5f, hexColor(0xFF3B5C) },
        { glm::vec3(0.0f, 5.4f, -7.0f), glm::vec3(0.0f, 5.4f, 7.5f), 2.0f, hexColor(0x3BD4FF) },
    };

    for (const Route& route : routes) {
        Drone drone;
        drone.path = make<LineMove>(clock, route.from, route.to, route.speed, true);
        drone.roll = make<Rotate>(0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
        drone.color = route.color;
        drone.returning = false;
        drone.rollTime = -1.0f;
        drone.boost = 0.0f;

        auto part = [&](const glm::vec3& offset, const glm::vec3& size) {
            TransformationComposite* t = makeComposite();
            t->addDynamic(drone.path);
            t->addDynamic(drone.roll);
            t->addStatic(Translate(offset));
            t->addStatic(Scale(size));
            return t;
        };

        spawn(part(glm::vec3(0.0f), glm::vec3(0.6f, 0.12f, 0.6f)), Material::chrome());
        spawn(part(glm::vec3(0.0f, 0.1f, 0.08f), glm::vec3(0.25f, 0.12f, 0.35f)), Material::pearl());
        drone.glow = spawn(part(glm::vec3(0.0f, -0.08f, 0.0f), glm::vec3(0.22f, 0.04f, 0.22f)), glowMaterial(), route.color, false, true);

        for (float sx : { -1.0f, 1.0f }) {
            for (float sz : { -1.0f, 1.0f }) {
                TransformationComposite* t = makeComposite();
                t->addDynamic(drone.path);
                t->addDynamic(drone.roll);
                t->addStatic(Translate(glm::vec3(sx * 0.32f, 0.1f, sz * 0.32f)));
                t->addDynamic(make<DynamicRotate>(clock, 1440.0f * sx * sz, UP));
                t->addStatic(Scale(glm::vec3(0.34f, 0.015f, 0.05f)));
                spawn(t, Material::silver());
            }
        }

        drone.marker = make<Translate>(glm::vec3(route.from.x, 0.02f, route.from.z));
        TransformationComposite* t = makeComposite();
        t->addDynamic(drone.marker);
        t->addStatic(Scale(glm::vec3(0.7f, 0.02f, 0.7f)));
        spawn(t, glowMaterial(), route.color * 0.35f, false, true);

        drone.light = addLight();
        drone.light->getTransformation().addDynamic(drone.path);
        drone.light->setPosition(glm::vec3(0.0f, -0.3f, 0.0f));
        drone.light->setColor(route.color);
        drone.light->setIntensity(3.0f);
        drone.light->setAttenuation(1.0f, 0.22f, 0.20f);

        drones.push_back(drone);
    }
}

void ShowcaseScene::buildLights() {
    SceneClock& clock = getClock();

    moonLight = addLight();
    moonLight->setType(LightType::Directional);
    moonLight->setDirection(glm::vec3(-0.35f, -1.0f, -0.25f));
    moonLight->setColor(hexColor(0x5A73BF));
    moonLight->setIntensity(0.55f);

    spotlight = addLight();
    spotlight->setType(LightType::Spot);
    spotlight->setPosition(glm::vec3(0.0f, 9.0f, 0.0f));
    spotlight->setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
    spotlight->setSpotAngles(5.0f, 9.0f);
    spotlight->setColor(hexColor(0xFFF4E0));
    spotlight->setIntensity(4.0f);
    spotlight->setAttenuation(1.0f, 0.01f, 0.002f);

    flashlight = addLight(false);
    flashlight->setType(LightType::Spot);
    flashlight->setSpotAngles(11.0f, 17.0f);
    flashlight->setColor(hexColor(0xFFF1D6));
    flashlight->setIntensity(3.5f);
    flashlight->setAttenuation(1.0f, 0.045f, 0.0075f);
    flashlight->setEnabled(false);

    const float radius = 3.3f;
    const float height = 0.5f;

    for (int i = 0; i < 3; ++i) {
        DynamicRotate* orbit = make<DynamicRotate>(clock, 40.0f, UP, 120.0f * static_cast<float>(i));

        Light* light = addLight();
        light->getTransformation().addStatic(Translate(glm::vec3(0.0f, height, 0.0f)));
        light->getTransformation().addDynamic(orbit);
        light->setPosition(glm::vec3(radius, 0.0f, 0.0f));
        light->setIntensity(2.2f);
        light->setAttenuation(1.0f, 0.35f, 0.44f);
        orbitLights.push_back(light);

        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(glm::vec3(0.0f, height, 0.0f)));
        t->addDynamic(orbit);
        t->addStatic(Translate(glm::vec3(radius, 0.0f, 0.0f)));
        t->addDynamic(make<DynamicRotate>(clock, 180.0f, glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
        t->addStatic(Scale(0.14f));
        orbitMarkers.push_back(spawn(t, glowMaterial()));
    }
}

void ShowcaseScene::buildGate() {
    const float pillarX = -3.2f;
    const float pillarHeight = 4.4f;
    const float lean = 0.18f;

    gateWobble = make<ShearWobble>();

    glm::mat4 shearMatrix(1.0f);
    shearMatrix[1][0] = lean;
    CustomMatrix* shear = make<CustomMatrix>(shearMatrix);
    CustomMatrix* mirror = make<CustomMatrix>(glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)));

    for (bool mirrored : { false, true }) {
        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(GATE_POSITION));
        t->addDynamic(gateWobble);
        if (mirrored) {
            t->addDynamic(mirror);
        }
        t->addStatic(Translate(glm::vec3(pillarX, 0.0f, 0.0f)));
        t->addDynamic(shear);
        t->addStatic(Translate(glm::vec3(0.0f, 0.5f * pillarHeight, 0.0f)));
        t->addStatic(Scale(glm::vec3(0.5f, pillarHeight, 0.5f)));

        DrawableObject* pillar = spawn(t, stoneMaterial());
        pillar->setTexture(crispTexture);
        gateParts.push_back(pillar);
    }

    const float topX = -pillarX - lean * pillarHeight + 0.25f;

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(GATE_POSITION));
    t->addDynamic(gateWobble);
    t->addStatic(Translate(glm::vec3(0.0f, pillarHeight + 0.15f, 0.0f)));
    t->addStatic(Scale(glm::vec3(2.0f * topX + 0.5f, 0.35f, 0.6f)));
    gateParts.push_back(spawn(t, Material::chrome()));

    t = makeComposite();
    t->addStatic(Translate(GATE_POSITION));
    t->addDynamic(gateWobble);
    t->addStatic(Translate(glm::vec3(0.0f, pillarHeight - 0.1f, 0.32f)));
    t->addStatic(Scale(glm::vec3(2.0f * topX - 0.4f, 0.06f, 0.04f)));
    gateNeon = spawn(t, glowMaterial(), glm::vec3(1.0f), false, true);
}

void ShowcaseScene::buildSky() {
    TransformationComposite* t = make<TransformationComposite>();
    t->addStatic(Scale(260.0f));
    dome = spawn(t, blackMaterial(), glm::vec3(0.0f), false, true);

    std::normal_distribution<float> gauss(0.0f, 1.0f);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    for (int i = 0; i < 380; ++i) {
        glm::vec3 direction(gauss(random), gauss(random), gauss(random));
        if (glm::length(direction) < 1e-3f) {
            continue;
        }
        direction = glm::normalize(direction);

        const float distance = 420.0f + 180.0f * unit(random);
        const float size = 1.0f + 2.0f * unit(random) * unit(random);

        t = make<TransformationComposite>();
        t->addStatic(Translate(direction * distance));
        t->addStatic(Rotate(360.0f * unit(random), glm::normalize(glm::vec3(unit(random), 1.0f, unit(random)))));
        t->addStatic(Scale(size));
        spawn(t, glowMaterial(), hue(unit(random), 0.25f * unit(random), 0.55f + 0.45f * unit(random)), false, true);
    }
}

void ShowcaseScene::buildHud() {
    for (std::size_t i = 0; i < ringColors.size(); ++i) {
        DrawableObject* swatch = hudSquare(HudAnchor::BottomLeft, glm::vec2(0.09f + 0.075f * static_cast<float>(i), 0.09f), 0.055f);
        swatch->setColor(ringColors[i]);
        hudSwatches.push_back(swatch);
    }

    for (int i = 0; i < 3; ++i) {
        hudEnvironment.push_back(hudSquare(HudAnchor::TopLeft, glm::vec2(0.09f + 0.085f * static_cast<float>(i), -0.09f), 0.065f));
    }

    for (int i = 0; i < COLLECTIBLE_COUNT; ++i) {
        hudCollected.push_back(hudSquare(HudAnchor::TopRight, glm::vec2(-0.09f - 0.07f * static_cast<float>(i), -0.09f), 0.05f));
    }

    for (int row = 0; row < 2; ++row) {
        for (int i = 0; i < TELEMETRY_LEDS; ++i) {
            const glm::vec2 offset(-0.09f - 0.05f * static_cast<float>(TELEMETRY_LEDS - 1 - i), 0.09f + 0.06f * static_cast<float>(row));
            hudTelemetry.push_back(hudSquare(HudAnchor::BottomRight, offset, 0.035f));
        }
    }

    for (int i = 0; i < SPEED_LEDS; ++i) {
        const glm::vec2 offset(-0.09f - 0.065f * static_cast<float>(SPEED_LEDS - 1 - i), 0.26f);
        hudSpeed.push_back(hudSquare(HudAnchor::BottomRight, offset, 0.045f));
    }

    hudCrosshair = hudSquare(HudAnchor::Center, glm::vec2(0.0f), 0.012f);
}

void ShowcaseScene::applyEnvironment() {
    Camera* camera = getCamera();
    DrawableObject* ground = getGround();

    glm::vec3 fogColor(0.0f);
    float fogDensity = 0.0f;
    float fogScatter = 0.0f;
    glm::vec3 rimColor(0.0f);
    float rimStrength = 0.0f;

    switch (environment) {
    case Environment::Night:
        setSkybox(resources.getSkybox(CubeMap::DEFAULT_DIRECTORY));
        setClearColor(hexColor(0x0B0F1A));
        rimColor = hexColor(0x5A73BF);
        rimStrength = 0.25f;
        camera->setFarPlane(250.0f);
        moonLight->setIntensity(0.55f);
        break;

    case Environment::Space:
        setSkybox(nullptr);
        setClearColor(hexColor(0x02030A));
        rimColor = hexColor(0x8A6CFF);
        rimStrength = 0.6f;
        camera->setFarPlane(900.0f);
        moonLight->setIntensity(0.25f);
        break;

    case Environment::Fog:
        setSkybox(nullptr);
        fogColor = hexColor(0x141A24);
        setClearColor(fogColor);
        fogDensity = 0.075f;
        fogScatter = 0.035f;
        camera->setFarPlane(250.0f);
        moonLight->setIntensity(0.3f);
        break;
    }

    if (ground) {
        ground->setVisible(environment != Environment::Space);
    }
    dome->setVisible(environment == Environment::Fog);

    litShader->use();
    litShader->setUniform("fogColor", fogColor);
    litShader->setUniform("fogDensity", fogDensity);
    litShader->setUniform("fogScatter", fogScatter);
    litShader->setUniform("rimColor", rimColor);
    litShader->setUniform("rimStrength", rimStrength);
}

const char* ShowcaseScene::cycleEnvironment() {
    environment = static_cast<Environment>((static_cast<int>(environment) + 1) % 3);
    applyEnvironment();

    switch (environment) {
    case Environment::Space:
        return "vesmir";
    case Environment::Fog:
        return "mlha";
    default:
        return "noc";
    }
}

bool ShowcaseScene::toggleMiniature() {
    miniature = !miniature;
    worldZoomTarget = miniature ? MINIATURE_ZOOM : 1.0f;
    worldLiftTarget = miniature ? MINIATURE_LIFT : 0.0f;
    return miniature;
}

void ShowcaseScene::nudgeWorld(float spinDeg, float lift) {
    worldSpinTarget += spinDeg;
    worldLiftTarget = glm::clamp(worldLiftTarget + lift, -3.0f, 8.0f);
}

void ShowcaseScene::resetWorld() {
    miniature = false;
    worldZoomTarget = 1.0f;
    worldLiftTarget = 0.0f;
    worldSpinTarget = 360.0f * std::round(worldSpinTarget / 360.0f);
}

void ShowcaseScene::updateWorld(float deltaTime) {
    if (miniature) {
        worldSpinTarget += MINIATURE_SPIN * deltaTime;
    }

    const float blend = 1.0f - std::exp(-2.5f * deltaTime);
    worldZoom += (worldZoomTarget - worldZoom) * blend;
    worldLift += (worldLiftTarget - worldLift) * blend;
    worldSpin += (worldSpinTarget - worldSpin) * blend;

    getZoom().setFactor(glm::vec3(worldZoom));
    getSpin().setAngle(worldSpin);
    getMove().setOffset(glm::vec3(0.0f, worldLift, 0.0f));
}

const char* ShowcaseScene::cyclePendulums() {
    pendulumMode = (pendulumMode + 1) % 3;
    const double time = getClock().getTime();
    const int count = static_cast<int>(pendulums.size());

    for (int i = 0; i < count; ++i) {
        float offset = 0.0f;
        if (pendulumMode == 1) {
            offset = 360.0f * static_cast<float>(i) / static_cast<float>(count);
        } else if (pendulumMode == 2) {
            offset = (i % 2 == 0) ? 0.0f : 180.0f;
        }

        const double phase = std::fmod(-360.0 * pendulums[i].frequency * time + offset, 360.0);
        pendulums[i].swing->setPhase(static_cast<float>(phase));
    }

    switch (pendulumMode) {
    case 1:
        return "spirala";
    case 2:
        return "nuzky";
    default:
        return "srovnana";
    }
}

void ShowcaseScene::supernova() {
    if (sunPhase != 0) {
        return;
    }

    sunBurst->setFrom(glm::vec3(1.0f));
    sunBurst->setTo(glm::vec3(2.4f));
    sunBurst->setDuration(0.35f);
    sunBurst->restart();
    sunPhase = 1;

    for (DrawableObject* cube : sunCubes) {
        flash(cube);
    }
}

void ShowcaseScene::updateSun() {
    if (sunPhase == 1 && sunBurst->isFinished()) {
        sunBurst->setFrom(sunBurst->getTo());
        sunBurst->setTo(glm::vec3(1.0f));
        sunBurst->setDuration(1.6f);
        sunBurst->restart();
        sunPhase = 2;
    } else if (sunPhase == 2 && sunBurst->isFinished()) {
        sunPhase = 0;
    }

    const float burst = sunBurst->getCurrentScale().x;
    sunLight->setIntensity(3.0f * burst * burst);
}

void ShowcaseScene::startCollectibles() {
    removeCollectibles();

    std::vector<glm::vec2> spots = {
        { -3.0f, 7.0f }, { 3.0f, 7.0f }, { 0.0f, 9.0f }, { -6.0f, 5.5f }, { 6.0f, 5.5f },
        { -4.5f, -6.0f }, { 4.5f, -6.0f }, { -6.5f, -4.5f }, { 6.5f, -4.5f }, { -2.3f, 1.6f },
        { 2.3f, -1.6f }, { -8.0f, 4.5f }, { 8.0f, 4.5f }, { 0.0f, -6.5f },
    };
    std::shuffle(spots.begin(), spots.end(), random);

    SceneClock& clock = getClock();
    collectTotal = COLLECTIBLE_COUNT;
    collectedCount = 0;
    celebrateUntil = -1.0f;

    for (int i = 0; i < COLLECTIBLE_COUNT; ++i) {
        const glm::vec3 base(spots[i].x, 0.7f, spots[i].y);

        Collectible collectible;
        collectible.bob = make<Translate>(glm::vec3(0.0f));
        collectible.grow = make<GrowScale>(clock, glm::vec3(0.001f), glm::vec3(1.0f), 0.5f + 0.1f * static_cast<float>(i));
        collectible.phase = 0.8f * static_cast<float>(i);
        collectible.state = 0;

        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(base));
        t->addDynamic(collectible.bob);
        t->addDynamic(collectible.grow);
        t->addDynamic(make<DynamicRotate>(clock, 110.0f, UP));
        t->addStatic(Rotate(45.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))));
        t->addStatic(Scale(0.3f));

        collectible.object = spawn(t, Material::gold(), glm::vec3(1.25f, 1.15f, 0.9f));
        collectible.object->setKind(DrawableObject::Kind::Collectible);

        collectible.light = addLight();
        collectible.light->setPosition(base + glm::vec3(0.0f, 0.5f, 0.0f));
        collectible.light->setColor(hexColor(0xFFD36B));
        collectible.light->setIntensity(1.6f);
        collectible.light->setAttenuation(1.0f, 0.7f, 1.8f);

        collectibles.push_back(collectible);
    }
}

void ShowcaseScene::removeCollectibles() {
    for (Collectible& collectible : collectibles) {
        forget(collectible.object);
        remove(collectible.object);
        removeLight(collectible.light);
    }
    collectibles.clear();
}

void ShowcaseScene::updateCollectibles(float deltaTime) {
    (void)deltaTime;
    const float time = static_cast<float>(getClock().getTime());

    for (auto it = collectibles.begin(); it != collectibles.end();) {
        Collectible& collectible = *it;
        collectible.bob->setOffset(glm::vec3(0.0f, 0.15f * std::sin(2.2f * time + collectible.phase), 0.0f));

        if (collectible.state == 1 && collectible.grow->isFinished()) {
            collectible.grow->setFrom(collectible.grow->getTo());
            collectible.grow->setTo(glm::vec3(0.001f));
            collectible.grow->setDuration(0.3f);
            collectible.grow->restart();
            collectible.state = 2;
        }

        if (collectible.state == 2 && collectible.grow->isFinished()) {
            forget(collectible.object);
            remove(collectible.object);
            removeLight(collectible.light);
            it = collectibles.erase(it);

            if (collectibles.empty() && collectedCount == collectTotal) {
                celebrateUntil = realTime + 4.0f;
                supernova();
                for (DrawableObject* cube : ringCubes) {
                    flash(cube);
                }
                for (float& boost : ringBoosts) {
                    boost = 1.0f;
                }
                std::cout << "Vsech " << collectTotal << " kostek sebrano! Supernova!" << std::endl;
            }
            continue;
        }

        const float scale = collectible.grow->getCurrentScale().x;
        collectible.light->setIntensity(1.6f * std::min(scale, 1.5f));
        ++it;
    }
}

void ShowcaseScene::updateDrones(float deltaTime) {
    const float clockDelta = getClock().getDelta();

    for (Drone& drone : drones) {
        const bool returning = drone.path->isReturning();
        if (returning != drone.returning) {
            drone.returning = returning;
            drone.rollTime = 0.0f;
            drone.boost = 1.0f;
        }

        if (drone.rollTime >= 0.0f) {
            drone.rollTime += clockDelta;
            const float progress = drone.rollTime / 0.8f;
            drone.roll->setAngle(360.0f * smooth01(progress));
            if (progress >= 1.0f) {
                drone.rollTime = -1.0f;
                drone.roll->setAngle(0.0f);
            }
        }

        drone.boost *= std::exp(-3.0f * deltaTime);
        drone.light->setIntensity(3.0f + 7.0f * drone.boost);
        drone.glow->setColor(glm::mix(drone.color, glm::vec3(1.6f), drone.boost));

        const glm::vec3 position = drone.path->getPosition();
        drone.marker->setOffset(glm::vec3(position.x, 0.02f, position.z));
    }
}

void ShowcaseScene::updateHud() {
    for (DrawableObject* swatch : hudSwatches) {
        swatch->setVisible(hudVisible);
    }

    for (int i = 0; i < 3; ++i) {
        const bool active = static_cast<int>(environment) == i;
        hudEnvironment[i]->setVisible(hudVisible);
        hudEnvironment[i]->setColor(ENVIRONMENT_COLORS[i] * (active ? 1.0f : 0.2f));
    }

    const bool showCollected = hudVisible && (!collectibles.empty() || realTime < celebrateUntil);
    for (int i = 0; i < COLLECTIBLE_COUNT; ++i) {
        DrawableObject* square = hudCollected[i];
        square->setVisible(showCollected && i < collectTotal);

        if (i < collectedCount) {
            const float sparkle = realTime < celebrateUntil ? 0.75f + 0.25f * std::sin(12.0f * realTime + static_cast<float>(i)) : 1.0f;
            square->setColor(hexColor(0xFFD36B) * sparkle);
        } else {
            square->setColor(glm::vec3(0.2f));
        }
    }

    for (std::size_t row = 0; row < drones.size() && row < 2; ++row) {
        const int lit = static_cast<int>(std::round(drones[row].path->getT() * static_cast<float>(TELEMETRY_LEDS - 1)));
        for (int i = 0; i < TELEMETRY_LEDS; ++i) {
            DrawableObject* led = hudTelemetry[row * TELEMETRY_LEDS + i];
            led->setVisible(hudVisible);
            led->setColor(drones[row].color * (i == lit ? 1.0f : 0.15f));
        }
    }

    const SceneClock& clock = getClock();
    const int level = glm::clamp(static_cast<int>(std::round(std::log2(clock.getSpeed()))) + 4, 1, SPEED_LEDS);
    for (int i = 0; i < SPEED_LEDS; ++i) {
        DrawableObject* led = hudSpeed[i];
        led->setVisible(hudVisible);

        if (clock.isPaused()) {
            led->setColor(hexColor(0xFF3B3B) * (0.4f + 0.6f * (0.5f + 0.5f * std::sin(6.0f * realTime))));
        } else {
            led->setColor(i < level ? hue(0.33f - 0.065f * static_cast<float>(i), 0.8f) : glm::vec3(0.15f));
        }
    }

    hudCrosshair->setVisible(hudVisible && flashlight->isEnabled());
}

void ShowcaseScene::update(float deltaTime) {
    realTime += deltaTime;
    const float time = static_cast<float>(getClock().getTime());

    updateWorld(deltaTime);

    for (const Bobber& bobber : bobbers) {
        bobber.offset->setOffset(glm::vec3(0.0f, 0.18f * std::sin(1.6f * time + bobber.phase), 0.0f));
    }

    const int ringCount = static_cast<int>(ringLights.size());
    for (int i = 0; i < ringCount; ++i) {
        if (ringBoosts[i] > 0.0f) {
            ringBoosts[i] *= std::exp(-1.3f * deltaTime);
            if (ringBoosts[i] < 0.002f) {
                ringBoosts[i] = 0.0f;
            }
            ringSpins[i]->setSpeed(ringSpinSpeeds[i] * (1.0f + 14.0f * ringBoosts[i]));
        }

        const float phase = 1.6f * time - TWO_PI * static_cast<float>(i) / static_cast<float>(ringCount);
        const float pulse = chaseEnabled ? std::pow(0.5f + 0.5f * std::cos(phase), 6.0f) : 0.6f;

        if (!ringLights[i]->isEnabled()) {
            ringTrims[i]->setColor(ringColors[i] * 0.08f);
            hudSwatches[i]->setColor(glm::vec3(0.18f));
            continue;
        }

        ringLights[i]->setIntensity(0.5f + 3.0f * pulse);
        ringTrims[i]->setColor(ringColors[i] * (0.25f + 0.85f * pulse));
        hudSwatches[i]->setColor(ringColors[i] * (0.3f + 0.7f * pulse));
    }

    if (spotlight && !ringPositions.empty()) {
        glm::vec3 target;

        if (spotlightFocus >= 0) {
            target = ringPositions[spotlightFocus];
            spotlight->setSpotAngles(7.0f, 11.0f);
        } else {
            const float angle = 0.5f * time + glm::half_pi<float>();
            target = glm::vec3(RING_RADIUS * std::cos(angle), RING_CUBE_HEIGHT, RING_RADIUS * std::sin(angle));
            spotlight->setSpotAngles(5.0f, 9.0f);
        }

        spotlight->setDirection(target - spotlight->getPosition());
    }

    if (flashlight->isEnabled()) {
        Camera* camera = getCamera();
        flashlight->setPosition(camera->getEye());
        flashlight->setDirection(camera->getTarget());
    }

    for (std::size_t i = 0; i < orbitLights.size(); ++i) {
        const glm::vec3 color = hue(0.08f * time + static_cast<float>(i) / 3.0f, 0.85f);
        orbitLights[i]->setColor(color);
        orbitMarkers[i]->setColor(color);
    }

    sunPulse->setFactor(glm::vec3(1.0f + 0.07f * std::sin(3.0f * time)));
    updateSun();

    for (const Column& column : columns) {
        const float ripple = 0.5f + 0.5f * std::sin(3.2f * time - 2.4f * column.distance);
        const float sweep = 0.5f + 0.5f * std::sin(1.3f * time + 1.7f * column.x);
        const float height = 0.12f + 1.5f * ripple * ripple + 0.45f * sweep;

        column.height->setFactor(glm::vec3(1.0f, height, 1.0f));
        column.object->setColor(hue(0.55f + 0.22f * height + 0.04f * time, 0.8f));
    }

    equalizerLight->setColor(hue(0.55f + 0.04f * time + 0.3f, 0.35f));

    const float step = std::min(deltaTime, 1.0f / 30.0f);
    gateWobble->velocity += (-60.0f * gateWobble->amount - 3.5f * gateWobble->velocity) * step;
    gateWobble->amount += gateWobble->velocity * step;
    gateNeon->setColor(hue(0.1f * realTime, 0.7f));

    updateDrones(deltaTime);
    updateSpawned(getClock().getDelta());
    updateCollectibles(deltaTime);
    updateHud();
    updateFlashes();
}

void ShowcaseScene::spawnCube(const glm::vec3& worldPosition) {
    int alive = 0;
    for (const Spawned& cube : spawned) {
        alive += cube.dying < 0.0f ? 1 : 0;
    }

    if (alive >= MAX_SPAWNED) {
        for (Spawned& cube : spawned) {
            if (cube.dying < 0.0f) {
                kill(cube);
                break;
            }
        }
    }

    const glm::vec3 local = glm::vec3(glm::inverse(getBase()->getMatrix()) * glm::vec4(worldPosition, 1.0f));

    const std::vector<Material> materials = materialPalette();
    std::uniform_int_distribution<int> pickMaterial(0, static_cast<int>(materials.size()) - 1);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f);

    Spawned cube;
    cube.base = glm::vec3(local.x, SPAWN_HEIGHT, local.z);
    cube.velocity = 0.0f;
    cube.squashAmount = 0.0f;
    cube.size = 0.45f + 0.35f * unit(random);
    cube.dying = -1.0f;
    cube.position = make<Translate>(cube.base);
    cube.squash = make<Scale>(glm::vec3(1.0f));

    TransformationComposite* t = makeComposite();
    t->addDynamic(cube.position);
    t->addDynamic(cube.squash);
    t->addStatic(Translate(glm::vec3(0.0f, 0.5f * cube.size, 0.0f)));
    t->addDynamic(make<DynamicRotate>(getClock(), 60.0f + 120.0f * unit(random), UP, 360.0f * unit(random)));
    t->addStatic(Scale(cube.size));
    cube.object = spawn(t, materials[pickMaterial(random)]);
    cube.object->setKind(DrawableObject::Kind::Moving);

    cube.light = addLight();
    cube.light->setColor(hue(unit(random), 0.7f));
    cube.light->setIntensity(2.2f);
    cube.light->setAttenuation(1.0f, 0.35f, 0.44f);
    cube.light->setPosition(cube.base + glm::vec3(0.0f, cube.size + 0.5f, 0.0f));

    spawned.push_back(cube);
}

void ShowcaseScene::kill(Spawned& cube) {
    if (cube.dying < 0.0f) {
        cube.dying = 0.0f;
    }
}

void ShowcaseScene::clearSpawned() {
    for (Spawned& cube : spawned) {
        kill(cube);
    }
}

void ShowcaseScene::updateSpawned(float deltaTime) {
    for (Spawned& cube : spawned) {
        if (cube.dying >= 0.0f) {
            cube.dying += std::max(deltaTime, 1.0f / 120.0f);
        } else {
            cube.velocity -= GRAVITY * deltaTime;
            cube.base.y += cube.velocity * deltaTime;

            if (cube.base.y <= 0.0f) {
                cube.base.y = 0.0f;
                if (cube.velocity < -1.5f) {
                    cube.squashAmount = std::min(0.4f, -cube.velocity * 0.04f);
                    cube.velocity = -cube.velocity * BOUNCE;
                } else {
                    cube.velocity = 0.0f;
                }
            }
        }

        cube.squashAmount *= std::exp(-9.0f * deltaTime);
        const float shrink = cube.dying >= 0.0f ? glm::clamp(1.0f - cube.dying / DIE_DURATION, 0.001f, 1.0f) : 1.0f;
        const float s = cube.squashAmount;

        cube.position->setOffset(cube.base);
        cube.squash->setFactor(shrink * glm::vec3(1.0f + 0.5f * s, 1.0f - s, 1.0f + 0.5f * s));
        cube.light->setPosition(cube.base + glm::vec3(0.0f, cube.size + 0.5f, 0.0f));
        cube.light->setIntensity(2.2f * shrink);
    }

    for (auto it = spawned.begin(); it != spawned.end();) {
        if (it->dying >= DIE_DURATION) {
            forget(it->object);
            remove(it->object);
            removeLight(it->light);
            it = spawned.erase(it);
        } else {
            ++it;
        }
    }
}

void ShowcaseScene::forget(DrawableObject* object) {
    flashes.erase(std::remove_if(flashes.begin(), flashes.end(), [object](const Flash& flash) { return flash.object == object; }), flashes.end());

    if (selected == object) {
        selected = nullptr;
    }
}

void ShowcaseScene::flash(DrawableObject* object) {
    for (Flash& existing : flashes) {
        if (existing.object == object) {
            existing.start = realTime;
            return;
        }
    }

    flashes.push_back({ object, object->getColor(), realTime });
}

void ShowcaseScene::updateFlashes() {
    for (auto it = flashes.begin(); it != flashes.end();) {
        const float k = 1.0f - (realTime - it->start) / FLASH_DURATION;

        if (k <= 0.0f) {
            it->object->setColor(it->baseColor);
            it = flashes.erase(it);
            continue;
        }

        it->object->setColor(it->baseColor * (1.0f + 2.5f * k * k));
        ++it;
    }
}

void ShowcaseScene::interact(DrawableObject* object) {
    selected = object;

    for (Spawned& cube : spawned) {
        if (cube.object == object) {
            kill(cube);
            return;
        }
    }

    if (object->getKind() == DrawableObject::Kind::Collectible) {
        for (Collectible& collectible : collectibles) {
            if (collectible.object == object && collectible.state == 0) {
                collectible.grow->setFrom(collectible.grow->getCurrentScale());
                collectible.grow->setTo(glm::vec3(1.8f));
                collectible.grow->setDuration(0.18f);
                collectible.grow->restart();
                collectible.state = 1;
                ++collectedCount;
                flash(object);
                std::cout << "Sebrano " << collectedCount << " / " << collectTotal << std::endl;
            }
        }
        return;
    }

    if (std::find(sunCubes.begin(), sunCubes.end(), object) != sunCubes.end()) {
        supernova();
        return;
    }

    if (std::find(gateParts.begin(), gateParts.end(), object) != gateParts.end()) {
        gateWobble->velocity += gateWobble->amount >= 0.0f ? 1.6f : -1.6f;
    }

    for (std::size_t i = 0; i < ringCubes.size(); ++i) {
        if (ringCubes[i] == object) {
            const int index = static_cast<int>(i);
            spotlightFocus = spotlightFocus == index ? -1 : index;
            ringBoosts[i] = 1.0f;
            break;
        }
    }

    flash(object);
}

void ShowcaseScene::hudClick(DrawableObject* object) {
    selected = object;

    for (std::size_t i = 0; i < hudSwatches.size(); ++i) {
        if (hudSwatches[i] == object) {
            ringLights[i]->setEnabled(!ringLights[i]->isEnabled());
            return;
        }
    }

    for (std::size_t i = 0; i < hudEnvironment.size(); ++i) {
        if (hudEnvironment[i] == object) {
            environment = static_cast<Environment>(i);
            applyEnvironment();
            return;
        }
    }
}

DrawableObject* ShowcaseScene::selectNext() {
    const unsigned start = selected ? selected->getId() : 0;

    for (unsigned step = 1; step <= 255; ++step) {
        const unsigned id = (start + step - 1) % 255 + 1;
        if (DrawableObject* object = getObjectById(id)) {
            selected = object;
            flash(object);
            return object;
        }
    }

    return nullptr;
}

void ShowcaseScene::printSelection() {
    if (!selected) {
        std::cout << "Neni vybrany zadny objekt (klikni na neco, nebo stiskni O)" << std::endl;
        return;
    }

    const glm::vec3 position = glm::vec3(selected->getTransformation()->getMatrix()[3]);
    const glm::vec3& color = selected->getColor();

    std::cout << "Objekt #" << selected->getId()
              << "  typ: " << DrawableObject::kindName(selected->getKind())
              << (isHud(selected) ? " (HUD)" : "")
              << "  pozice: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << "  barva: (" << color.r << ", " << color.g << ", " << color.b << ")";

    if (auto* composite = dynamic_cast<TransformationComposite*>(selected->getTransformation())) {
        std::cout << "  transformace [" << composite->describe() << "] (" << composite->size() << " kroku)";
    }

    std::cout << std::endl;
}

void ShowcaseScene::replayIntro() {
    for (GrowScale* animation : introAnimations) {
        animation->restart();
    }

    pendulumMode = 2;
    cyclePendulums();
}

bool ShowcaseScene::toggleFlashlight() {
    flashlight->setEnabled(!flashlight->isEnabled());
    return flashlight->isEnabled();
}

bool ShowcaseScene::toggleChase() {
    chaseEnabled = !chaseEnabled;
    return chaseEnabled;
}

bool ShowcaseScene::toggleHud() {
    hudVisible = !hudVisible;
    return hudVisible;
}
