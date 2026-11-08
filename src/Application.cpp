// Author: Matt Hejtman, login: HEJ0094
#include "Application.h"
#include "Camera.h"
#include "DrawableObject.h"
#include "CubeMap.h"
#include "CustomMatrix.h"
#include "DynamicRotate.h"
#include "Material.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Rotate.h"
#include "Scale.h"
#include "Scene.h"
#include "Skybox.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "TextureManager.h"
#include "TransformationComposite.h"
#include "Translate.h"
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "BasicModels.h"
#include "bushes.h"
#include "clock.h"
#include "earth.h"
#include "login.h"
#include "loginModel.h"
#include "moon.h"
#include "plain.h"
#include "rocket.h"
#include "sphere.h"
#include "sun.h"
#include "tree.h"

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <random>

Application::Application() = default;

Application::~Application() = default;

void Application::errorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void Application::initialization() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(800, 600, "ZPG", nullptr, nullptr);
    
    if (!window) {
        std::cerr << "ERROR: could not create window" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "ERROR: could not load OpenGL functions (GLAD)" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL:           " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GLFW:           " << glfwGetVersionString() << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    int textureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
    std::cout << "Texture units:  " << textureUnits << std::endl;

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwGetCursorPos(window, &lastCursorX, &lastCursorY);
}

static const int TREE_WOOD_VERTEX_COUNT = 36444;

static const float CLOCK_DIRECTION = -1.0f;

ShaderProgram* Application::createProgram(const char* vertexPath, const char* fragmentPath) {
    shaderPrograms.push_back(std::make_unique<ShaderProgram>(vertexPath, fragmentPath));

    return shaderPrograms.back().get();
}

void Application::createShaders() {
    colorProgram = createProgram("assets/shaders/model.vert", "assets/shaders/color.frag");
    normalProgram = createProgram("assets/shaders/model.vert", "assets/shaders/normal.frag");
    treeProgram = createProgram("assets/shaders/foliage.vert", "assets/shaders/color.frag");
    bushProgram = createProgram("assets/shaders/foliage.vert", "assets/shaders/color.frag");
    clockProgram = createProgram("assets/shaders/clock.vert", "assets/shaders/color.frag");
    hudProgram = createProgram("assets/shaders/model.vert", "assets/shaders/color.frag");
    constantProgram = createProgram("assets/shaders/lit.vert", "assets/shaders/constant.frag");
    lambertProgram = createProgram("assets/shaders/lit.vert", "assets/shaders/lambert.frag");
    phongProgram = createProgram("assets/shaders/lit.vert", "assets/shaders/phong.frag");
    blinnProgram = createProgram("assets/shaders/lit.vert", "assets/shaders/blinn.frag");
    nightTreeProgram = createProgram("assets/shaders/litFoliage.vert", "assets/shaders/phong.frag");
    nightBushProgram = createProgram("assets/shaders/litFoliage.vert", "assets/shaders/phong.frag");
    skyboxProgram = createProgram("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");

    treeProgram->setUniform("woodVertexCount", TREE_WOOD_VERTEX_COUNT);
    bushProgram->setUniform("woodVertexCount", 0);
    nightTreeProgram->setUniform("woodVertexCount", TREE_WOOD_VERTEX_COUNT);
    nightBushProgram->setUniform("woodVertexCount", 0);

    cameraPrograms = {colorProgram, normalProgram, treeProgram, bushProgram, clockProgram,
                      constantProgram, lambertProgram, phongProgram, blinnProgram,
                      nightTreeProgram, nightBushProgram, skyboxProgram};

    litPrograms = {lambertProgram, phongProgram, blinnProgram, nightTreeProgram, nightBushProgram};
}

TransformationComposite* Application::makeComposite(Scene* scene) {
    TransformationComposite* composite = make<TransformationComposite>();
    composite->addDynamic(scene->getBase());

    return composite;
}

DrawableObject* Application::createObject(Model* model, ShaderProgram* program, Transformation* transformation) {
    objects.push_back(std::make_unique<DrawableObject>(model, program, transformation));

    return objects.back().get();
}

Scene* Application::createScene() {
    scenes.push_back(std::make_unique<Scene>());
    Scene* scene = scenes.back().get();

    Camera* camera = scene->getCamera();

    for (ShaderProgram* program : cameraPrograms) {
        camera->attach(program);
    }

    for (ShaderProgram* program : litPrograms) {
        scene->addLitProgram(program);
    }

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    camera->setAspect(width, height);
    camera->notifyAll();
    return scene;
}

void Application::addSignature(Scene* scene) {
    TransformationComposite* signature = make<TransformationComposite>();
    signature->addStatic(Translate(glm::vec3(0.77f, -0.93f, -0.9f)));
    signature->addStatic(Scale(0.2f));
    scene->addStatic(createObject(loginColorModel, hudProgram, signature));
}

void Application::addClock(Scene* scene, const glm::vec3& position, float angleY, float scale)
{
    TransformationComposite* transformation = makeComposite(scene);
    transformation->addStatic(Translate(position));
    transformation->addStatic(Rotate(180.0f + angleY, glm::vec3(0.0f, 1.0f, 0.0f)));
    transformation->addStatic(Scale(glm::vec3(-scale, scale, scale)));
    scene->add(createObject(clockModelGpu, clockProgram, transformation));
}

static const glm::vec3 FOREST_CLOCK_POSITIONS[] = {glm::vec3(0.0f, 2.2f, 8.0f), glm::vec3(-6.0f, 2.8f, 6.0f), glm::vec3(7.0f, 2.0f, 4.0f)};
static const float FOREST_CLOCK_ANGLES[] = {0.0f, 26.6f, -26.6f};
static const float FOREST_CLOCK_SCALES[] = {1.0f, 1.5f, 1.2f};

void Application::plantForest(Scene* scene, ShaderProgram* treeShader, ShaderProgram* bushShader, const Material& material) {
    std::mt19937 generator(42);
    std::uniform_real_distribution<float> position(-40.0f, 40.0f);
    std::uniform_real_distribution<float> angle(0.0f, 360.0f);
    std::uniform_real_distribution<float> treeScale(0.3f, 0.6f);
    std::uniform_real_distribution<float> bushScale(1.5f, 3.0f);

    auto isFree = [&](float x, float z) {
        if (glm::length(glm::vec2(x, z - 18.0f)) < 6.0f) {
            return false;
        }

        for (const glm::vec3& c : FOREST_CLOCK_POSITIONS) {
            if (glm::length(glm::vec2(x - c.x, z - c.z)) < 4.0f) {
                return false;
            }
            
            return true;
        }
    };

    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    auto plant = [&](Model* model, ShaderProgram* program, std::uniform_real_distribution<float>& scaleDistribution) {
        float x = 0.0f, z = 0.0f;

        do {
            x = position(generator);
            z = position(generator);
        } while (!isFree(x, z));

        const float rotation = angle(generator);
        const float scale = scaleDistribution(generator);

        TransformationComposite* transformation = makeComposite(scene);
        transformation->addStatic(Translate(glm::vec3(x, 0.0f, z)));
        transformation->addStatic(Rotate(rotation, up));
        transformation->addStatic(Scale(scale));
        DrawableObject* object = createObject(model, program, transformation);
        object->setMaterial(material);
        scene->add(object);
    };

    const int TREE_COUNT = 80;
    const int BUSH_COUNT = 60;
    for (int i = 0; i < TREE_COUNT; ++i) {
        plant(treeModel, treeShader, treeScale);
    }

    for (int i = 0; i < BUSH_COUNT; ++i) {
        plant(bushModel, bushShader, bushScale);
    }
}

void Application::createForestScene() {
    Scene* scene = createScene();
    scene->getCamera()->setSpeed(6.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 1.7f, 18.0f), 92.0f, -90.0f);

    TransformationComposite* groundTransformation = makeComposite(scene);
    groundTransformation->addStatic(Scale(glm::vec3(50.0f, 1.0f, 50.0f)));
    scene->add(createObject(groundModel, colorProgram, groundTransformation));

    plantForest(scene, treeProgram, bushProgram, Material());
    scene->setSkybox(getSkybox());

    for (int i = 0; i < 3; ++i) {
        addClock(scene, FOREST_CLOCK_POSITIONS[i], FOREST_CLOCK_ANGLES[i], FOREST_CLOCK_SCALES[i]);
    }

    addSignature(scene);
}

void Application::createClockScene() {
    Scene* scene = createScene();
    addClock(scene, glm::vec3(0.0f), 0.0f, 1.0f);
    addSignature(scene);
}

void Application::createSolarSystemScene() {
    Scene* scene = createScene();
    scene->getLight()->setPosition(glm::vec3(0.0f));
    scene->getCamera()->setPosition(glm::vec3(0.0f, 1.0f, 2.0f), 116.0f, -90.0f);
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    TransformationComposite* sun = makeComposite(scene);
    sun->addDynamic(make<DynamicRotate>(10.0f, up));
    sun->addStatic(Scale(0.3f));
    scene->add(createObject(sunModel, constantProgram, sun));

    TransformationComposite* earthOrbit = make<TransformationComposite>();
    earthOrbit->addDynamic(make<DynamicRotate>(20.0f, up));
    earthOrbit->addStatic(Translate(glm::vec3(0.6f, 0.0f, 0.0f)));

    TransformationComposite* earth = makeComposite(scene);
    earth->addDynamic(earthOrbit);
    earth->addDynamic(make<DynamicRotate>(90.0f, up));
    earth->addStatic(Scale(0.1f));
    scene->add(createObject(earthModel, phongProgram, earth));

    TransformationComposite* moon = makeComposite(scene);
    moon->addDynamic(earthOrbit);
    moon->addDynamic(make<DynamicRotate>(60.0f, up));
    moon->addStatic(Translate(glm::vec3(0.2f, 0.0f, 0.0f)));
    moon->addStatic(Scale(0.04f));
    scene->add(createObject(moonModel, phongProgram, moon));

    TransformationComposite* rocket = makeComposite(scene);
    rocket->addDynamic(earthOrbit);
    rocket->addDynamic(make<DynamicRotate>(-120.0f, up));
    rocket->addStatic(Translate(glm::vec3(0.3f, 0.0f, 0.0f)));
    rocket->addStatic(Scale(0.06f));
    scene->add(createObject(rocketModel, blinnProgram, rocket));

    addSignature(scene);
}

void Application::createOrderScene() {
    Scene* scene = createScene();
    DynamicRotate* spin = make<DynamicRotate>(45.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    TransformationComposite* spinInPlace = makeComposite(scene);
    spinInPlace->addStatic(Translate(glm::vec3(-0.5f, 0.0f, 0.0f)));
    spinInPlace->addDynamic(spin);
    scene->add(createObject(centeredTriangleModel, colorProgram, spinInPlace));

    TransformationComposite* orbit = makeComposite(scene);
    orbit->addDynamic(spin);
    orbit->addStatic(Translate(glm::vec3(0.5f, 0.0f, 0.0f)));
    scene->add(createObject(centeredTriangleModel, colorProgram, orbit));

    addSignature(scene);
}

void Application::createModelMatrixScene() {
    Scene* scene = createScene();
    TransformationComposite* transformation = makeComposite(scene);
    transformation->addStatic(Translate(glm::vec3(0.2f, 0.1f, 0.0f)));
    transformation->addStatic(Rotate(30.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    transformation->addStatic(Scale(2.5f));
    scene->add(createObject(centeredTriangleModel, colorProgram, transformation));
    addSignature(scene);
}

void Application::createLoginScene() {
    Scene* scene = createScene();
    TransformationComposite* transformation = makeComposite(scene);
    transformation->addStatic(Scale(0.9f));
    scene->add(createObject(loginTextModel, normalProgram, transformation));
}

void Application::createSpheresScene() {
    Scene* scene = createScene();
    scene->getLight()->setPosition(glm::vec3(0.0f));
    scene->getCamera()->setPosition(glm::vec3(0.0f, 0.0f, 8.0f), 90.0f, -90.0f);

    TransformationComposite* marker = makeComposite(scene);
    marker->addStatic(Scale(0.1f));
    scene->add(createObject(sphereModel, constantProgram, marker));

    const glm::vec3 positions[] = {
        glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, -2.0f, 0.0f)
    };

    for (const glm::vec3& position : positions) {
        TransformationComposite* transformation = makeComposite(scene);
        transformation->addStatic(Translate(position));
        DrawableObject* sphereObject = createObject(sphereModel, phongProgram, transformation);
        sphereObject->setColor(glm::vec3(0.35f, 0.6f, 1.0f));
        scene->add(sphereObject);
        testSpheres.push_back(sphereObject);
    }

    addSignature(scene);
}

void Application::createNormalsScene() {
    Scene* scene = createScene();
    scene->getLight()->setPosition(glm::vec3(3.0f, 3.0f, 4.0f));
    scene->getLight()->setAttenuation(1.0f, 0.0f, 0.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 0.0f, 7.0f), 90.0f, -90.0f);

    TransformationComposite* ellipsoid = makeComposite(scene);
    ellipsoid->addStatic(Rotate(30.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    ellipsoid->addStatic(Scale(glm::vec3(2.0f, 1.0f, 1.0f)));
    DrawableObject* ellipsoidObject = createObject(sphereModel, phongProgram, ellipsoid);
    ellipsoidObject->setColor(glm::vec3(1.0f, 0.55f, 0.15f));
    scene->add(ellipsoidObject);

    glm::mat4 wMatrix(1.0f);
    wMatrix[3][3] = 5.0f;
    TransformationComposite* wSphere = makeComposite(scene);
    wSphere->addStatic(Translate(glm::vec3(2.8f, -1.6f, 0.0f)));
    wSphere->addStatic(CustomMatrix(wMatrix));
    DrawableObject* wObject = createObject(sphereModel, phongProgram, wSphere);
    wObject->setColor(glm::vec3(0.35f, 0.6f, 1.0f));
    scene->add(wObject);

    TransformationComposite* referenceSphere = makeComposite(scene);
    referenceSphere->addStatic(Translate(glm::vec3(2.2f, -1.6f, 0.0f)));
    referenceSphere->addStatic(Scale(0.2f));
    DrawableObject* referenceObject = createObject(sphereModel, phongProgram, referenceSphere);
    referenceObject->setColor(glm::vec3(0.35f, 0.6f, 1.0f));
    scene->add(referenceObject);

    addSignature(scene);
}

void Application::createBackSpecularScene() {
    Scene* scene = createScene();
    scene->getLight()->setPosition(glm::vec3(0.0f, 0.0f, -4.0f));
    scene->getLight()->setAttenuation(1.0f, 0.0f, 0.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 0.0f, 5.0f), 90.0f, -90.0f);

    Material wide;
    wide.shininess = 1.0f;
    TransformationComposite* transformation = makeComposite(scene);
    DrawableObject* sphereObject = createObject(sphereModel, phongProgram, transformation);
    sphereObject->setColor(glm::vec3(0.35f, 0.6f, 1.0f));
    sphereObject->setMaterial(wide);
    scene->add(sphereObject);

    addSignature(scene);
}

void Application::createNightForestScene()
{
    Scene* scene = createScene();
    scene->setClearColor(glm::vec3(0.01f, 0.01f, 0.03f)); 
    scene->getCamera()->setSpeed(6.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 2.0f, 10.0f), 96.0f, -90.0f);

    Material night;
    night.ambient = glm::vec3(0.02f);
    night.diffuse = glm::vec3(1.0f);
    night.specular = glm::vec3(0.15f);
    night.shininess = 16.0f;

    TransformationComposite* groundTransformation = makeComposite(scene);
    groundTransformation->addStatic(Scale(glm::vec3(50.0f, 1.0f, 50.0f)));
    DrawableObject* ground = createObject(plainModel, phongProgram, groundTransformation);
    ground->setColor(glm::vec3(0.15f, 0.35f, 0.12f));
    ground->setMaterial(night);
    scene->add(ground);

    plantForest(scene, nightTreeProgram, nightBushProgram, night);

    const glm::vec3 fireflyColor(0.85f, 1.0f, 0.4f);
    TransformationComposite* firefly = makeComposite(scene);
    firefly->addStatic(Translate(glm::vec3(0.0f, 1.5f, -2.0f)));
    firefly->addDynamic(make<DynamicRotate>(30.0f, glm::vec3(0.0f, 1.0f, 0.0f)));
    firefly->addStatic(Translate(glm::vec3(5.0f, 0.0f, 0.0f)));
    firefly->addStatic(Scale(0.08f));
    DrawableObject* fireflyObject = createObject(sphereModel, constantProgram, firefly);
    fireflyObject->setColor(fireflyColor);
    scene->add(fireflyObject);

    scene->getLight()->setPosition(glm::vec3(0.0f));
    scene->getLight()->setColor(fireflyColor);
    scene->getLight()->setAttenuation(1.0f, 0.35f, 0.44f);
    scene->setLightTransformation(firefly);

    addSignature(scene);
}

std::string Application::pathOrPlaceholder(const std::string& path, const std::string& placeholder) {
    if (std::filesystem::exists(path)) {
        return path;
    }

    std::cout << "Not found: " << path << " -> using test placeholder " << placeholder << std::endl;

    return placeholder;
}

Skybox* Application::getSkybox() {

    if (!skyboxes.empty()) {
        return skyboxes.front().get();
    }

    static const char* FACES[] = {"posx", "negx", "posy", "negy", "posz", "negz"};
    static const char* PLACEHOLDERS[] = {"right", "left", "top", "bottom", "front", "back"};
    std::array<std::string, 6> paths;

    for (int i = 0; i < 6; ++i) {
        paths[i] = pathOrPlaceholder(std::string("assets/skybox/cubemap/") + FACES[i] + ".jpg", std::string("assets/test/skybox/") + PLACEHOLDERS[i] + ".png");
    }

    cubeMaps.push_back(std::make_unique<CubeMap>(paths));
    skyboxes.push_back(std::make_unique<Skybox>(skycubeModel, skyboxProgram, cubeMaps.back().get()));
    return skyboxes.back().get();
}

Bounds Application::loadObject(Scene* scene, const std::string& path, const glm::vec3& position, float rotationY, float targetHeight, ShaderProgram* program, const Material& material) {
    LoadedModel loaded = ModelLoader::load(path);
    const glm::vec3 size = loaded.boundsMax - loaded.boundsMin;

    TransformationComposite* transformation = makeComposite(scene);
    transformation->addStatic(Translate(position));
    transformation->addStatic(Rotate(rotationY, glm::vec3(0.0f, 1.0f, 0.0f)));
    float scale = 1.0f;
    if (targetHeight > 0.0f && size.y > 0.0f) {
        scale = targetHeight / size.y;
        const glm::vec3 baseCenter((loaded.boundsMin.x + loaded.boundsMax.x) * 0.5f, loaded.boundsMin.y, (loaded.boundsMin.z + loaded.boundsMax.z) * 0.5f);
        transformation->addStatic(Scale(scale));
        transformation->addStatic(Translate(-baseCenter));
    }

    for (ModelPart& part : loaded.parts) {
        Model* model = part.model.get();
        models.push_back(std::move(part.model));
        DrawableObject* object = createObject(model, program, transformation);
        object->setMaterial(material);

        if (!part.texturePath.empty() && std::filesystem::exists(part.texturePath)) {
            object->setTexture(textureManager->get(part.texturePath));
        }
        else {
            if (!part.texturePath.empty()) {
                std::cout << "  texture not found: " << part.texturePath << std::endl;
            }

            object->setColor(part.diffuseColor);
        }
        scene->add(object);
    }

    Bounds bounds;
    if (targetHeight > 0.0f) {
        bounds.min = position + glm::vec3(-size.x, 0.0f, -size.z) * 0.5f * scale;
        bounds.max = position + glm::vec3(size.x, 2.0f * size.y, size.z) * 0.5f * scale;
    } else {
        bounds.min = position + loaded.boundsMin;
        bounds.max = position + loaded.boundsMax;
    }

    return bounds;
}

void Application::createShrekScene() {
    Scene* scene = createScene();
    scene->setSkybox(getSkybox());
    scene->getLight()->setPosition(glm::vec3(1.5f, 3.0f, 3.0f));
    scene->getLight()->setAttenuation(1.0f, 0.0f, 0.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 1.0f, 2.6f), 95.0f, -90.0f);

    Material material;
    material.ambient = glm::vec3(0.25f);
    material.specular = glm::vec3(0.3f);

    loadObject(scene, pathOrPlaceholder("assets/shrek/shrek.obj", "assets/test/test_a.obj"), glm::vec3(0.0f), 0.0f, 1.8f, phongProgram, material);

    addSignature(scene);
}

void Application::createTextureScene() {
    Scene* scene = createScene();
    scene->setSkybox(getSkybox());
    scene->getLight()->setPosition(glm::vec3(0.0f, 5.0f, 4.0f));
    scene->getLight()->setAttenuation(1.0f, 0.0f, 0.0f);
    scene->getCamera()->setSpeed(3.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 1.6f, 6.0f), 98.0f, -90.0f);

    Material material;
    material.ambient = glm::vec3(0.25f);
    material.specular = glm::vec3(0.3f);

    DrawableObject* floor = createObject(floorModel, phongProgram, makeComposite(scene));
    floor->setMaterial(material);
    floor->setTexture(textureManager->get("assets/Texture/wooden_fence.png"));
    scene->add(floor);

    loadObject(scene, pathOrPlaceholder("assets/shrek/shrek.obj", "assets/test/test_a.obj"), glm::vec3(-2.5f, 0.0f, 0.0f), 0.0f, 1.8f, phongProgram, material);
    loadObject(scene, pathOrPlaceholder("assets/shrek/fiona.obj", "assets/test/test_b.obj"), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.6f, phongProgram, material);
    loadObject(scene, pathOrPlaceholder("assets/shrek/toiled.obj", "assets/test/test_c.obj"), glm::vec3(2.5f, 0.0f, 0.0f), 0.0f, 0.9f, phongProgram, material);

    addSignature(scene);
}

void Application::createClassroomScene() {
    Scene* scene = createScene();
    scene->setSkybox(getSkybox());

    Material material;
    material.ambient = glm::vec3(0.3f);
    material.specular = glm::vec3(0.2f);
    material.shininess = 16.0f;

    const Bounds room = loadObject(scene, pathOrPlaceholder("assets/EB408/eb408.obj", "assets/test/classroom.obj"), glm::vec3(0.0f), 0.0f, 0.0f, phongProgram, material);

    const glm::vec3 center = (room.min + room.max) * 0.5f;

    loadObject(scene, pathOrPlaceholder("assets/shrek/shrek.obj", "assets/test/test_a.obj"), glm::vec3(room.min.x + 0.9f, room.min.y, center.z - 1.8f), 90.0f, 1.8f, phongProgram, material);

    scene->getLight()->setPosition(glm::vec3(center.x, std::min(room.max.y - 0.3f, room.min.y + 2.6f), center.z));
    scene->getLight()->setAttenuation(1.0f, 0.045f, 0.0075f);

    scene->getCamera()->setSpeed(2.5f);
    scene->getCamera()->setPosition(glm::vec3(room.max.x - 0.5f, room.min.y + 1.6f, center.z), 97.0f, 180.0f);
    addSignature(scene);
}

void Application::setSpheresProgram(ShaderProgram* program, const char* name) {
    for (DrawableObject* sphereObject : testSpheres) {
        sphereObject->setProgram(program);
    }

    std::cout << "Spheres: " << name << std::endl;
}

void Application::createModels() {
    auto addModel = [this](const float* data, std::size_t floatCount, int floatsPerVertex = 6, int attribute1Offset = 3, int attribute2Offset = -1) {
        models.push_back(std::make_unique<Model>(data, floatCount, floatsPerVertex, attribute1Offset, attribute2Offset));
        return models.back().get();
    };

    centeredTriangleModel = addModel(centeredTriangle, sizeof(centeredTriangle) / sizeof(float));
    groundModel = addModel(ground, sizeof(ground) / sizeof(float));
    plainModel = addModel(plain, sizeof(plain) / sizeof(float));
    floorModel = addModel(texturedFloor, sizeof(texturedFloor) / sizeof(float), 8);
    sphereModel = addModel(sphere, sizeof(sphere) / sizeof(float));
    treeModel = addModel(tree, sizeof(tree) / sizeof(float));
    bushModel = addModel(bushes, sizeof(bushes) / sizeof(float));
    loginTextModel = addModel(loginModel, sizeof(loginModel) / sizeof(float));
    loginColorModel = addModel(login, sizeof(login) / sizeof(float), 9, 6);
    sunModel = addModel(sun_vertices, sizeof(sun_vertices) / sizeof(float), 9, 3, 6);
    earthModel = addModel(earth_vertices, sizeof(earth_vertices) / sizeof(float), 9, 3, 6);
    moonModel = addModel(moon_vertices, sizeof(moon_vertices) / sizeof(float), 9, 3, 6);
    rocketModel = addModel(rocket, sizeof(rocket) / sizeof(float), 9, 3, 6);
    clockModelGpu = addModel(clockModel, sizeof(clockModel) / sizeof(float), 9, 6);
    skycubeModel = addModel(skycube, sizeof(skycube) / sizeof(float), 3);
    textureManager = std::make_unique<TextureManager>();

    createForestScene();
    createClockScene();
    createSolarSystemScene();
    createOrderScene();
    createModelMatrixScene();
    createLoginScene();
    createSpheresScene();
    createNormalsScene();
    createBackSpecularScene();
    createNightForestScene();
    createShrekScene();
    createTextureScene();
    createClassroomScene();

    activateScene(0);
}

void Application::activateScene(std::size_t index)
{
    if (index >= scenes.size()) {
        return;
    }

    activeScene = index;

    Camera* camera = scenes[activeScene]->getCamera();
    if (correctResize){
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        camera->setAspect(width, height);
    }
    camera->notifyAll();
}

void Application::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    switch (key) {
    case GLFW_KEY_C: app->setSpheresProgram(app->constantProgram, "constant"); return;
    case GLFW_KEY_L: app->setSpheresProgram(app->lambertProgram, "Lambert"); return;
    case GLFW_KEY_P: app->setSpheresProgram(app->phongProgram, "Phong"); return;
    case GLFW_KEY_B: app->setSpheresProgram(app->blinnProgram, "Blinn-Phong"); return;
    default: break;
    }

    if (key == GLFW_KEY_TAB && !app->scenes.empty()) {
        const std::size_t count = app->scenes.size();
        const std::size_t next = (mods & GLFW_MOD_SHIFT) ? (app->activeScene + count - 1) % count : (app->activeScene + 1) % count;
        app->activateScene(next);
        std::cout << "Active scene: " << next + 1 << std::endl;
        return;
    }

    if (key == GLFW_KEY_U) {
        app->uvTest = !app->uvTest;
        app->phongProgram->setUniform("uvTest", app->uvTest ? 1 : 0);
        std::cout << "UV test: " << (app->uvTest ? "on" : "off") << std::endl;
        return;
    }

    if (key == GLFW_KEY_N) {
        app->normalMode = (app->normalMode + 1) % 3;
        static const char* NAMES[] = {"0 = untransformed (wrong)", "1 = model matrix (wrong)", "2 = normal matrix transpose(inverse(M)) (correct)"};
        
        for (ShaderProgram* program : {app->lambertProgram, app->phongProgram, app->blinnProgram}) {
            program->setUniform("normalMode", app->normalMode);
        }

        std::cout << "Normals: " << NAMES[app->normalMode] << std::endl;
        return;
    }
    if (key == GLFW_KEY_O) {
        app->cutBackSpecular = !app->cutBackSpecular;
        app->phongProgram->setUniform("cutBackSpecular", app->cutBackSpecular ? 1 : 0);
        std::cout << "Phong back-face specular cut: " << (app->cutBackSpecular ? "on" : "off") << std::endl;
        return;
    }
    if (key == GLFW_KEY_V) {
        app->correctResize = !app->correctResize;
        std::cout << "Resize updates aspect: " << (app->correctResize ? "yes (correct)" : "no (stretched)") << std::endl;
        
        if (app->correctResize){
            app->activateScene(app->activeScene);
        }
        return;
    }

    if (key == GLFW_KEY_F && app->activeScene < app->scenes.size()) {
        static const float FOVS[] = {30.0f, 45.0f, 60.0f, 90.0f, 120.0f, 150.0f, 170.0f};
        app->fovIndex = (app->fovIndex + 1) % (sizeof(FOVS) / sizeof(FOVS[0]));
        app->scenes[app->activeScene]->getCamera()->setFov(FOVS[app->fovIndex]);
        std::cout << "FOV: " << FOVS[app->fovIndex] << " deg" << std::endl;
        return;
    }

    if ((key >= GLFW_KEY_1 && key <= GLFW_KEY_9) || key == GLFW_KEY_0) {
        const std::size_t index = key == GLFW_KEY_0 ? 9 : static_cast<std::size_t>(key - GLFW_KEY_1);
        if (index < app->scenes.size()) {
            app->activateScene(index);
            std::cout << "Active scene: " << index + 1 << std::endl;
        }
    }
}

void Application::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app) {
        return;
    }

    const double dx = x - app->lastCursorX;
    const double dy = y - app->lastCursorY;
    app->lastCursorX = x;
    app->lastCursorY = y;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && app->activeScene < app->scenes.size()) {
        app->scenes[app->activeScene]->getCamera()->rotate(static_cast<float>(dx), static_cast<float>(dy));
    }
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    glViewport(0, 0, width, height);
    if (app && app->correctResize && app->activeScene < app->scenes.size()) {
        app->scenes[app->activeScene]->getCamera()->setAspect(width, height);
    }
}

void Application::updateClock() {
    const std::time_t now = std::time(nullptr);
    std::tm local{};

#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif

    const float seconds = static_cast<float>(local.tm_sec);
    const float minutes = static_cast<float>(local.tm_min);
    const float hours = static_cast<float>(local.tm_hour % 12);

    const float secondDeg = seconds * 6.0f;
    const float minuteDeg = minutes * 6.0f + seconds * 0.1f;
    const float hourDeg = hours * 30.0f + minutes * 0.5f;

    clockProgram->setUniform("secondAngle", CLOCK_DIRECTION * glm::radians(secondDeg));
    clockProgram->setUniform("minuteAngle", CLOCK_DIRECTION * glm::radians(minuteDeg));
    clockProgram->setUniform("hourAngle", CLOCK_DIRECTION * glm::radians(hourDeg));
}

void Application::run() {
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const float deltaTime = static_cast<float>(now - lastTime);
        lastTime = now;
        processInput(deltaTime);
        updateClock();

        if (activeScene < scenes.size()) {
            const glm::vec3& background = scenes[activeScene]->getClearColor();
            glClearColor(background.r, background.g, background.b, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (activeScene < scenes.size()) {
            scenes[activeScene]->draw();
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    releaseResources();
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}

void Application::processInput(float deltaTime) {
    if (activeScene >= scenes.size()) {
        return;
    }

    Scene* scene = scenes[activeScene].get();

    auto pressed = [this](int key) { return glfwGetKey(window, key) == GLFW_PRESS; };

    Camera* camera = scene->getCamera();
    if (pressed(GLFW_KEY_W)) camera->moveForward(deltaTime);
    if (pressed(GLFW_KEY_S)) camera->moveBackward(deltaTime);
    if (pressed(GLFW_KEY_A)) camera->moveLeft(deltaTime);
    if (pressed(GLFW_KEY_D)) camera->moveRight(deltaTime);
    if (pressed(GLFW_KEY_E)) camera->moveUp(deltaTime);
    if (pressed(GLFW_KEY_Q)) camera->moveDown(deltaTime);

    const float moveSpeed = 0.8f;
    const float zoomSpeed = 1.5f;
    const float spinSpeed = 90.0f;

    glm::vec3 move(0.0f);
    if (pressed(GLFW_KEY_LEFT))  move.x -= moveSpeed * deltaTime;
    if (pressed(GLFW_KEY_RIGHT)) move.x += moveSpeed * deltaTime;
    if (pressed(GLFW_KEY_DOWN))  move.y -= moveSpeed * deltaTime;
    if (pressed(GLFW_KEY_UP))    move.y += moveSpeed * deltaTime;

    float zoom = 1.0f;
    if (pressed(GLFW_KEY_KP_ADD) || pressed(GLFW_KEY_EQUAL)) {
        zoom *= std::pow(zoomSpeed, deltaTime);
    }

    if (pressed(GLFW_KEY_KP_SUBTRACT) || pressed(GLFW_KEY_MINUS)) {
        zoom /= std::pow(zoomSpeed, deltaTime);
    }

    float spin = 0.0f;
    if (pressed(GLFW_KEY_R)) {
        const bool shift = pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
        spin = (shift ? -spinSpeed : spinSpeed) * deltaTime;
    }

    scene->getMove().setOffset(scene->getMove().getOffset() + move);
    scene->getSpin().setAngle(scene->getSpin().getAngle() + spin);
    scene->getZoom().setFactor(scene->getZoom().getFactor() * zoom);
}

void Application::releaseResources() {
    scenes.clear();
    skyboxes.clear();
    cubeMaps.clear();

    if (textureManager) {
        textureManager->clear();
    }
    
    objects.clear();
    transformations.clear();
    models.clear();
    shaderPrograms.clear();
    cameraPrograms.clear();
    litPrograms.clear();
    testSpheres.clear();
    colorProgram = normalProgram = treeProgram = bushProgram = clockProgram = hudProgram = nullptr;
    constantProgram = lambertProgram = phongProgram = blinnProgram = nullptr;
    nightTreeProgram = nightBushProgram = skyboxProgram = nullptr;
    centeredTriangleModel = groundModel = plainModel = floorModel = sphereModel = treeModel = bushModel = nullptr;
    loginTextModel = loginColorModel = sunModel = earthModel = moonModel = clockModelGpu = rocketModel = nullptr;
    skycubeModel = nullptr;
}
