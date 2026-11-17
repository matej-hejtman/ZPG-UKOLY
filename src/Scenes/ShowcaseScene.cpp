// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/ShowcaseScene.h"
#include "Core/Resources.h"
#include "Lighting/Material.h"
#include "Rendering/DrawableObject.h"
#include "Transformations/DynamicRotate.h"
#include "Transformations/GrowScale.h"
#include "Transformations/LineMove.h"
#include "Transformations/Oscillate.h"

#include <glm/gtc/constants.hpp>

#include <cmath>
#include <vector>

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

static GrowScale* growFrom(Scene& scene, float durationSeconds, const glm::vec3& from = glm::vec3(0.001f), const glm::vec3& pivot = glm::vec3(0.0f)) {
    return scene.make<GrowScale>(scene.getClock(), from, glm::vec3(1.0f), durationSeconds, pivot);
}

static const glm::vec3 SUN_POSITION = glm::vec3(0.0f, 3.4f, 0.0f);
static const float RING_RADIUS = 5.0f;
static const float RING_CUBE_HEIGHT = 1.75f;
static const glm::vec3 HELIX_CENTER = glm::vec3(-8.0f, 0.0f, -1.0f);
static const glm::vec3 EQUALIZER_CENTER = glm::vec3(8.0f, 0.0f, -1.0f);
static const float PENDULUM_Z = -8.5f;
static const float PENDULUM_PIVOT_Y = 4.6f;

DrawableObject* ShowcaseScene::spawn(Transformation* transformation, const Material& material, const glm::vec3& color, bool textured, bool decorative) {
    DrawableObject* object = decorative ? createStaticObject(cubeModel, litShader, transformation) : createObject(cubeModel, litShader, transformation);
    object->setMaterial(material);
    object->setColor(color);

    if (textured) {
        object->setTexture(defaultTexture);
    }

    return object;
}

void ShowcaseScene::create() {
    cubeModel = resources.getModel("cube");
    litShader = resources.getShader("lit.vert", "phong.frag");
    defaultTexture = resources.getTexture(Texture::DEFAULT_PATH);

    setAmbientLight(glm::vec3(AMBIENT));
    setClearColor(hexColor(0x0B0F1A));

    getCamera()->setPosition(glm::vec3(0.0f, 5.5f, 14.0f), 104.0f, -90.0f);
    getCamera()->setSpeed(3.0f);

    if (DrawableObject* ground = getGround()) {
        ground->setColor(glm::vec3(0.55f));
    }

    buildSolarSystem();
    buildMaterialRing();
    buildPendulumWave();
    buildHelix();
    buildEqualizer();
    buildDrones();
    buildLights();
    buildHud();
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
    GrowScale* sunGrow = growFrom(*this, 1.6f);

    t = makeComposite();
    t->addStatic(Translate(SUN_POSITION));
    t->addDynamic(sunGrow);
    t->addDynamic(sunPulse);
    t->addDynamic(make<DynamicRotate>(clock, 25.0f, UP));
    t->addStatic(Scale(0.85f));
    spawn(t, glowMaterial(), hexColor(0xFFC94A));

    t = makeComposite();
    t->addStatic(Translate(SUN_POSITION));
    t->addDynamic(sunGrow);
    t->addDynamic(sunPulse);
    t->addDynamic(make<DynamicRotate>(clock, -40.0f, glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f))));
    t->addStatic(Rotate(45.0f, glm::vec3(1.0f, 0.0f, 1.0f)));
    t->addStatic(Scale(0.85f));
    spawn(t, glowMaterial(), hexColor(0xFF7A2E));

    Light* sun = addLight();
    sun->setPosition(SUN_POSITION);
    sun->setColor(hexColor(0xFFC37A));
    sun->setIntensity(3.0f);
    sun->setAttenuation(1.0f, 0.09f, 0.032f);

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
        GrowScale* grow = growFrom(*this, 1.5f + 0.4f * static_cast<float>(i));

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

    const std::vector<Material> materials = {
        Material::gold(), Material::silver(), Material::copper(), Material::bronze(), Material::chrome(),
        Material::pearl(), Material::emerald(), Material::ruby(), Material::plastic(), Material::rubber(),
    };

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

        t = makeComposite();
        t->addStatic(Translate(ringPositions.back()));
        t->addDynamic(bob);
        t->addDynamic(growFrom(*this, 0.8f + 0.18f * static_cast<float>(i)));
        t->addDynamic(make<DynamicRotate>(clock, 40.0f + 6.0f * static_cast<float>(i), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f))));
        t->addStatic(Scale(0.7f));
        spawn(t, materials[i]);

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

        TransformationComposite* rod = makeComposite();
        rod->addStatic(Translate(pivot));
        rod->addDynamic(swing);
        rod->addStatic(Translate(glm::vec3(0.0f, -0.5f * length, 0.0f)));
        rod->addStatic(Scale(glm::vec3(0.03f, length, 0.03f)));
        spawn(rod, Material::silver());

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

    GrowScale* grow = growFrom(*this, 3.0f, glm::vec3(1.0f, 0.001f, 1.0f));
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
        spawn(t, Material::pearl());
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

    struct Drone {
        glm::vec3 from;
        glm::vec3 to;
        float speed;
        glm::vec3 color;
    };

    const std::vector<Drone> drones = {
        { glm::vec3(-9.0f, 2.6f, 7.5f), glm::vec3(9.0f, 2.6f, 7.5f), 2.5f, hexColor(0xFF3B5C) },
        { glm::vec3(0.0f, 5.4f, -7.0f), glm::vec3(0.0f, 5.4f, 7.5f), 2.0f, hexColor(0x3BD4FF) },
    };

    for (const Drone& drone : drones) {
        LineMove* path = make<LineMove>(clock, drone.from, drone.to, drone.speed, true);

        TransformationComposite* t = makeComposite();
        t->addDynamic(path);
        t->addStatic(Scale(glm::vec3(0.6f, 0.12f, 0.6f)));
        spawn(t, Material::chrome());

        t = makeComposite();
        t->addDynamic(path);
        t->addStatic(Translate(glm::vec3(0.0f, 0.1f, 0.08f)));
        t->addStatic(Scale(glm::vec3(0.25f, 0.12f, 0.35f)));
        spawn(t, Material::pearl());

        t = makeComposite();
        t->addDynamic(path);
        t->addStatic(Translate(glm::vec3(0.0f, -0.08f, 0.0f)));
        t->addStatic(Scale(glm::vec3(0.22f, 0.04f, 0.22f)));
        spawn(t, glowMaterial(), drone.color, false, true);

        for (float sx : { -1.0f, 1.0f }) {
            for (float sz : { -1.0f, 1.0f }) {
                t = makeComposite();
                t->addDynamic(path);
                t->addStatic(Translate(glm::vec3(sx * 0.32f, 0.1f, sz * 0.32f)));
                t->addDynamic(make<DynamicRotate>(clock, 1440.0f * sx * sz, UP));
                t->addStatic(Scale(glm::vec3(0.34f, 0.015f, 0.05f)));
                spawn(t, Material::silver());
            }
        }

        Light* light = addLight();
        light->getTransformation().addDynamic(path);
        light->setPosition(glm::vec3(0.0f, -0.3f, 0.0f));
        light->setColor(drone.color);
        light->setIntensity(3.0f);
        light->setAttenuation(1.0f, 0.22f, 0.20f);
    }
}

void ShowcaseScene::buildLights() {
    SceneClock& clock = getClock();

    Light* moon = addLight();
    moon->setType(LightType::Directional);
    moon->setDirection(glm::vec3(-0.35f, -1.0f, -0.25f));
    moon->setColor(hexColor(0x5A73BF));
    moon->setIntensity(0.55f);

    spotlight = addLight();
    spotlight->setType(LightType::Spot);
    spotlight->setPosition(glm::vec3(0.0f, 9.0f, 0.0f));
    spotlight->setDirection(glm::vec3(0.0f, -1.0f, 0.0f));
    spotlight->setSpotAngles(5.0f, 9.0f);
    spotlight->setColor(hexColor(0xFFF4E0));
    spotlight->setIntensity(4.0f);
    spotlight->setAttenuation(1.0f, 0.01f, 0.002f);

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

void ShowcaseScene::buildHud() {
    for (std::size_t i = 0; i < ringColors.size(); ++i) {
        const glm::vec2 offset(0.09f + 0.075f * static_cast<float>(i), 0.09f);
        DrawableObject* swatch = createHudObject(cubeModel, HudAnchor::BottomLeft, offset, 0.055f);
        swatch->setColor(ringColors[i]);
        hudSwatches.push_back(swatch);
    }
}

void ShowcaseScene::update(float deltaTime) {
    (void)deltaTime;
    const float time = static_cast<float>(getClock().getTime());

    for (const Bobber& bobber : bobbers) {
        bobber.offset->setOffset(glm::vec3(0.0f, 0.18f * std::sin(1.6f * time + bobber.phase), 0.0f));
    }

    const int ringCount = static_cast<int>(ringLights.size());
    for (int i = 0; i < ringCount; ++i) {
        const float phase = 1.6f * time - TWO_PI * static_cast<float>(i) / static_cast<float>(ringCount);
        const float pulse = std::pow(0.5f + 0.5f * std::cos(phase), 6.0f);

        ringLights[i]->setIntensity(0.5f + 3.0f * pulse);
        ringTrims[i]->setColor(ringColors[i] * (0.25f + 0.85f * pulse));
        hudSwatches[i]->setColor(ringColors[i] * (0.3f + 0.7f * pulse));
    }

    if (spotlight && !ringPositions.empty()) {
        const float angle = 0.5f * time + glm::half_pi<float>();
        const glm::vec3 target(RING_RADIUS * std::cos(angle), RING_CUBE_HEIGHT, RING_RADIUS * std::sin(angle));
        spotlight->setDirection(target - spotlight->getPosition());
    }

    for (std::size_t i = 0; i < orbitLights.size(); ++i) {
        const glm::vec3 color = hue(0.08f * time + static_cast<float>(i) / 3.0f, 0.85f);
        orbitLights[i]->setColor(color);
        orbitMarkers[i]->setColor(color);
    }

    if (sunPulse) {
        sunPulse->setFactor(glm::vec3(1.0f + 0.07f * std::sin(3.0f * time)));
    }

    for (const Column& column : columns) {
        const float ripple = 0.5f + 0.5f * std::sin(3.2f * time - 2.4f * column.distance);
        const float sweep = 0.5f + 0.5f * std::sin(1.3f * time + 1.7f * column.x);
        const float height = 0.12f + 1.5f * ripple * ripple + 0.45f * sweep;

        column.height->setFactor(glm::vec3(1.0f, height, 1.0f));
        column.object->setColor(hue(0.55f + 0.22f * height + 0.04f * time, 0.8f));
    }

    if (equalizerLight) {
        equalizerLight->setColor(hue(0.55f + 0.04f * time + 0.3f, 0.35f));
    }
}
