// Author: Matt Hejtman, login: HEJ0094
#include "Application.h"
#include "Camera.h"
#include "DrawableObject.h"
#include "CubeMap.h"
#include "CustomMatrix.h"
#include "DynamicRotate.h"
#include "GrowScale.h"
#include "LineMove.h"
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
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
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
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

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
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    int textureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureUnits);
    std::cout << "Texture units:  " << textureUnits << std::endl;

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
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
    DrawableObject* ground = createObject(groundModel, colorProgram, groundTransformation);
    ground->setKind(DrawableObject::Kind::Ground);
    scene->add(ground);

    plantForest(scene, treeProgram, bushProgram, Material());
    addCollectibles(scene);
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
    nightForestScene = scene;
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
    ground->setKind(DrawableObject::Kind::Ground);
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

Bounds Application::loadObject(Scene* scene, const std::string& path, const glm::vec3& position, float rotationY, float targetHeight, ShaderProgram* program, const Material& material, DrawableObject::Kind kind, bool useBase, std::vector<DrawableObject*>* created) {
    LoadedModel loaded = ModelLoader::load(path);
    const glm::vec3 size = loaded.boundsMax - loaded.boundsMin;

    TransformationComposite* transformation = useBase ? makeComposite(scene) : make<TransformationComposite>();
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
        object->setKind(kind);

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
        if (created) {
            created->push_back(object);
        }
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
    classroomScene = scene;
    scene->setSkybox(getSkybox());

    Material material;
    material.ambient = glm::vec3(0.3f);
    material.specular = glm::vec3(0.2f);
    material.shininess = 16.0f;

    const Bounds room = loadObject(scene, pathOrPlaceholder("assets/EB408/eb408.obj", "assets/test/classroom.obj"), glm::vec3(0.0f), 0.0f, 0.0f, phongProgram, material, DrawableObject::Kind::Ground);

    const glm::vec3 center = (room.min + room.max) * 0.5f;

    loadObject(scene, pathOrPlaceholder("assets/shrek/shrek.obj", "assets/test/test_a.obj"), glm::vec3(room.min.x + 0.9f, room.min.y, center.z - 1.8f), 90.0f, 1.8f, phongProgram, material);

    scene->getLight()->setPosition(glm::vec3(center.x, std::min(room.max.y - 0.3f, room.min.y + 2.6f), center.z));
    scene->getLight()->setAttenuation(1.0f, 0.045f, 0.0075f);

    scene->getCamera()->setSpeed(2.5f);
    scene->getCamera()->setPosition(glm::vec3(room.max.x - 0.5f, room.min.y + 1.6f, center.z), 97.0f, 180.0f);
    addSignature(scene);
}

void Application::createTerrainScene() {
    Scene* scene = createScene();
    terrainScene = scene;
    scene->setSkybox(getSkybox());
    scene->getLight()->setPosition(glm::vec3(20.0f, 60.0f, 30.0f));
    scene->getLight()->setAttenuation(1.0f, 0.0f, 0.0f);
    scene->getCamera()->setSpeed(8.0f);
    scene->getCamera()->setPosition(glm::vec3(0.0f, 22.0f, 38.0f), 112.0f, -90.0f);

    Material material;
    material.ambient = glm::vec3(0.3f);
    material.specular = glm::vec3(0.05f);
    material.shininess = 8.0f;

    loadObject(scene, pathOrPlaceholder("assets/teren/teren.obj", "assets/test/test_a.obj"), glm::vec3(0.0f), 0.0f, 15.0f, phongProgram, material,
               DrawableObject::Kind::Ground);
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
    createTerrainScene();

    activateScene(0);
}

void Application::activateScene(std::size_t index)
{
    if (index >= scenes.size()) {
        return;
    }

    clearInteraction();
    activeScene = index;

    Camera* camera = scenes[activeScene]->getCamera();
    if (correctResize){
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        camera->setAspect(width, height);
    }
    camera->notifyAll();
}

static std::size_t wrapIndex(std::size_t index, int step, std::size_t count) {
    return static_cast<std::size_t>((static_cast<long>(index) + step % static_cast<long>(count) + static_cast<long>(count)) % static_cast<long>(count));
}

void Application::printHelp() const {
    std::cout <<
        "=== Controls ===\n"
        "F1          this help\n"
        "F2 / Sh+F2  left mouse mode: select -> insert -> path (M)\n"
        "F3 / Sh+F3  lighting of the four spheres, scene 7: constant / Lambert / Phong / Blinn-Phong (C L P B)\n"
        "F4 / Sh+F4  field of view: next / previous (F)\n"
        "F5 / Sh+F5  normal transformation: none / model matrix / normal matrix (N)\n"
        "F6          Phong back-face specular cut on/off (O)\n"
        "F7          window resize updates aspect / only viewport (V)\n"
        "F8          UV test (U)\n"
        "F9 / Sh+F9  next / previous scene (TAB / Shift+TAB), 1-9 and 0 = scenes 1-10\n"
        "F10         reset scene move / spin / zoom (arrows, + -, R)\n"
        "Sh+F10      remove objects inserted by clicks in this scene\n"
        "F11         fullscreen on/off\n"
        "F12         print current settings\n"
        "WASD, E/Q, right mouse   camera\n"
        "left mouse  select / insert / path point (by mode)\n"
        "Esc         quit" << std::endl;
}

void Application::printStatus() const {
    static const char* SPHERES[] = {"constant", "Lambert", "Phong", "Blinn-Phong"};
    std::size_t inserted = 0;
    for (const auto& entry : insertedObjects) {
        if (activeScene < scenes.size() && entry.first == scenes[activeScene].get()) {
            ++inserted;
        }
    }
    std::cout << "=== Status ===\n"
              << "scene " << activeScene + 1 << " / " << scenes.size()
              << ", mode " << modeName(interactionMode) << (hasPathStart ? " (point A set)" : "")
              << ", inserted objects " << inserted << ", collected " << collected << " / " << collectibleTotal << "\n"
              << "spheres " << SPHERES[spheresProgramIndex]
              << ", FOV " << (activeScene < scenes.size() ? scenes[activeScene]->getCamera()->getFov() : 0.0f) << " deg"
              << ", normals " << normalMode
              << ", back specular cut " << (cutBackSpecular ? "on" : "off")
              << ", resize aspect " << (correctResize ? "on" : "off")
              << ", UV test " << (uvTest ? "on" : "off")
              << ", fullscreen " << (fullscreen ? "on" : "off") << std::endl;
}

void Application::cycleMode(int step) {
    interactionMode = static_cast<InteractionMode>(wrapIndex(static_cast<std::size_t>(interactionMode), step, 3));
    hasPathStart = false;
    std::cout << "Mode: " << modeName(interactionMode) << std::endl;
}

void Application::cycleSpheresProgram(int step) {
    ShaderProgram* programs[] = {constantProgram, lambertProgram, phongProgram, blinnProgram};
    static const char* NAMES[] = {"constant", "Lambert", "Phong", "Blinn-Phong"};
    spheresProgramIndex = wrapIndex(spheresProgramIndex, step, 4);
    setSpheresProgram(programs[spheresProgramIndex], NAMES[spheresProgramIndex]);
}

void Application::cycleFov(int step) {
    if (activeScene >= scenes.size()) {
        return;
    }
    static const float FOVS[] = {30.0f, 45.0f, 60.0f, 90.0f, 120.0f, 150.0f, 170.0f};
    fovIndex = wrapIndex(fovIndex, step, sizeof(FOVS) / sizeof(FOVS[0]));
    scenes[activeScene]->getCamera()->setFov(FOVS[fovIndex]);
    std::cout << "FOV: " << FOVS[fovIndex] << " deg" << std::endl;
}

void Application::cycleNormalMode(int step) {
    static const char* NAMES[] = {"0 = untransformed (wrong)", "1 = model matrix (wrong)", "2 = normal matrix transpose(inverse(M)) (correct)"};
    normalMode = static_cast<int>(wrapIndex(static_cast<std::size_t>(normalMode), step, 3));
    for (ShaderProgram* program : {lambertProgram, phongProgram, blinnProgram}) {
        program->setUniform("normalMode", normalMode);
    }
    std::cout << "Normals: " << NAMES[normalMode] << std::endl;
}

void Application::toggleBackSpecular() {
    cutBackSpecular = !cutBackSpecular;
    phongProgram->setUniform("cutBackSpecular", cutBackSpecular ? 1 : 0);
    std::cout << "Phong back-face specular cut: " << (cutBackSpecular ? "on" : "off") << std::endl;
}

void Application::toggleCorrectResize() {
    correctResize = !correctResize;
    std::cout << "Resize updates aspect: " << (correctResize ? "yes (correct)" : "no (stretched)") << std::endl;
    if (correctResize) {
        activateScene(activeScene);
    }
}

void Application::toggleUvTest() {
    uvTest = !uvTest;
    phongProgram->setUniform("uvTest", uvTest ? 1 : 0);
    std::cout << "UV test: " << (uvTest ? "on" : "off") << std::endl;
}

void Application::switchScene(int step) {
    if (scenes.empty()) {
        return;
    }
    const std::size_t next = wrapIndex(activeScene, step, scenes.size());
    activateScene(next);
    std::cout << "Active scene: " << next + 1 << std::endl;
}

void Application::resetSceneControls() {
    if (activeScene >= scenes.size()) {
        return;
    }
    Scene* scene = scenes[activeScene].get();
    scene->getMove().setOffset(glm::vec3(0.0f));
    scene->getSpin().setAngle(0.0f);
    scene->getZoom().setFactor(glm::vec3(1.0f));
    std::cout << "Scene move / spin / zoom reset" << std::endl;
}

void Application::removeInsertedObjects() {
    if (activeScene >= scenes.size()) {
        return;
    }
    Scene* scene = scenes[activeScene].get();
    std::size_t removed = 0;
    for (auto it = insertedObjects.begin(); it != insertedObjects.end();) {
        if (it->first == scene) {
            if (it->second == selectedObject) {
                selectObject(nullptr);
            }
            if (scene->remove(it->second)) {
                ++removed;
            }
            it = insertedObjects.erase(it);
        }
        else {
            ++it;
        }
    }
    hasPathStart = false;
    std::cout << "Removed " << removed << " inserted objects" << std::endl;
}

void Application::toggleFullscreen() {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    if (!mode) {
        return;
    }

    fullscreen = !fullscreen;
    if (fullscreen) {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
        glfwSetWindowSize(window, windowedWidth, windowedHeight);
    }
    std::cout << "Fullscreen: " << (fullscreen ? "on" : "off") << std::endl;
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

    const int dir = (mods & GLFW_MOD_SHIFT) ? -1 : 1;
    const bool shift = dir < 0;

    switch (key) {
    case GLFW_KEY_F1: app->printHelp(); return;
    case GLFW_KEY_F2: app->cycleMode(dir); return;
    case GLFW_KEY_F3: app->cycleSpheresProgram(dir); return;
    case GLFW_KEY_F4: app->cycleFov(dir); return;
    case GLFW_KEY_F5: app->cycleNormalMode(dir); return;
    case GLFW_KEY_F6: app->toggleBackSpecular(); return;
    case GLFW_KEY_F7: app->toggleCorrectResize(); return;
    case GLFW_KEY_F8: app->toggleUvTest(); return;
    case GLFW_KEY_F9: app->switchScene(dir); return;
    case GLFW_KEY_F10: if (shift) app->removeInsertedObjects(); else app->resetSceneControls(); return;
    case GLFW_KEY_F11: app->toggleFullscreen(); return;
    case GLFW_KEY_F12: app->printStatus(); return;

    case GLFW_KEY_C: app->spheresProgramIndex = 0; app->setSpheresProgram(app->constantProgram, "constant"); return;
    case GLFW_KEY_L: app->spheresProgramIndex = 1; app->setSpheresProgram(app->lambertProgram, "Lambert"); return;
    case GLFW_KEY_P: app->spheresProgramIndex = 2; app->setSpheresProgram(app->phongProgram, "Phong"); return;
    case GLFW_KEY_B: app->spheresProgramIndex = 3; app->setSpheresProgram(app->blinnProgram, "Blinn-Phong"); return;
    case GLFW_KEY_TAB: app->switchScene(dir); return;
    case GLFW_KEY_M: app->cycleMode(dir); return;
    case GLFW_KEY_U: app->toggleUvTest(); return;
    case GLFW_KEY_N: app->cycleNormalMode(1); return;
    case GLFW_KEY_O: app->toggleBackSpecular(); return;
    case GLFW_KEY_V: app->toggleCorrectResize(); return;
    case GLFW_KEY_F: app->cycleFov(1); return;
    default: break;
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

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (!app || button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) {
        return;
    }

    glfwGetCursorPos(window, &app->clickX, &app->clickY);
    app->clickPending = true;
}

const char* Application::modeName(InteractionMode mode) {
    switch (mode) {
    case InteractionMode::Insert: return "insert";
    case InteractionMode::Path: return "path";
    default: return "select";
    }
}

void Application::selectObject(DrawableObject* object) {
    if (selectedObject) {
        selectedObject->setColor(selectedOriginalColor);
    }

    selectedObject = object;
    if (selectedObject) {
        selectedOriginalColor = selectedObject->getColor();
        selectedObject->setColor(selectedOriginalColor * glm::vec3(1.0f, 0.4f, 0.4f));
    }
}

void Application::clearInteraction() {
    selectObject(nullptr);
    hasPathStart = false;
}

void Application::handleClick() {
    if (activeScene >= scenes.size()) {
        return;
    }
    Scene* scene = scenes[activeScene].get();
    Camera* camera = scene->getCamera();

    int windowWidth = 0, windowHeight = 0, width = 0, height = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &width, &height);
    if (windowWidth <= 0 || windowHeight <= 0 || width <= 0 || height <= 0) {
        return;
    }

    const int x = static_cast<int>(clickX * width / windowWidth);
    const int y = static_cast<int>(clickY * height / windowHeight);
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }
    const int newy = height - 1 - y;

    GLubyte color[4] = {0, 0, 0, 0};
    GLfloat depth = 1.0f;
    GLuint index = 0;
    glReadPixels(x, newy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glReadPixels(x, newy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels(x, newy, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    std::printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
                x, y, color[0], color[1], color[2], color[3], depth, index);

    DrawableObject* object = scene->getObjectById(index);

    const bool hasPoint = depth < 1.0f;
    glm::vec3 position(0.0f);
    if (hasPoint) {
        const glm::vec3 screen(static_cast<float>(x), static_cast<float>(newy), depth);
        const glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
        position = glm::unProject(screen, camera->getViewMatrix(), camera->getProjectionMatrix(), viewport);
        std::printf("unProject [%f, %f, %f]\n", position.x, position.y, position.z);
    }

    switch (interactionMode) {
    case InteractionMode::Select:
        if (!object) {
            std::cout << "Nothing selected (background)" << std::endl;
            selectObject(nullptr);
        }
        else if (object->getKind() == DrawableObject::Kind::Moving) {
            std::cout << "Pohybujici se objekt " << index << " odstranen" << std::endl;
            scene->remove(object);
        }
        else if (object->getKind() == DrawableObject::Kind::Collectible) {
            scene->remove(object);
            ++collected;
            std::cout << "Sebrano " << collected << " / " << collectibleTotal << std::endl;
        }
        else {
            std::cout << "Selected object " << index << " (" << DrawableObject::kindName(object->getKind()) << ")" << std::endl;
            selectObject(object->getKind() == DrawableObject::Kind::Ground ? nullptr : object);
        }
        break;

    case InteractionMode::Insert:
        if (!hasPoint || !object || object->getKind() != DrawableObject::Kind::Ground) {
            std::cout << "Insert: click on the ground" << std::endl;
            break;
        }
        insertModel(scene, position);
        break;

    case InteractionMode::Path:
        if (!hasPoint) {
            std::cout << "Path: click on an object, not on the sky" << std::endl;
            break;
        }
        if (!hasPathStart) {
            pathStart = position;
            hasPathStart = true;
            std::printf("Path point A [%f, %f, %f]\n", position.x, position.y, position.z);
        }
        else {
            std::printf("Path point B [%f, %f, %f]\n", position.x, position.y, position.z);
            addMovingObject(scene, pathStart, position);
            hasPathStart = false;
        }
        break;
    }
}

void Application::insertModel(Scene* scene, const glm::vec3& position) {
    if (scene == classroomScene) {
        Material material;
        material.ambient = glm::vec3(0.3f);
        material.specular = glm::vec3(0.2f);
        material.shininess = 16.0f;
        static std::mt19937 generator(9);
        std::uniform_real_distribution<float> angle(0.0f, 360.0f);
        std::vector<DrawableObject*> parts;
        loadObject(scene, pathOrPlaceholder("assets/shrek/shrek.obj", "assets/test/test_a.obj"), position, angle(generator), 0.9f, phongProgram, material, DrawableObject::Kind::Normal, false, &parts);
        for (DrawableObject* part : parts) {
            insertedObjects.emplace_back(scene, part);
        }
        std::cout << "Inserted Shrek" << std::endl;
        return;
    }

    static std::mt19937 generator(5);
    std::uniform_real_distribution<float> angle(0.0f, 360.0f);
    std::uniform_real_distribution<float> scale(0.3f, 0.6f);

    TransformationComposite* transformation = make<TransformationComposite>();
    transformation->addStatic(Translate(position));
    transformation->addStatic(Rotate(angle(generator), glm::vec3(0.0f, 1.0f, 0.0f)));
    const float size = scene == terrainScene ? 3.0f : 1.0f;
    transformation->addDynamic(make<GrowScale>(size * scale(generator), 2.0f));

    const bool night = scene == nightForestScene;
    DrawableObject* tree = createObject(treeModel, night ? nightTreeProgram : treeProgram, transformation);
    if (night) {
        Material material;
        material.ambient = glm::vec3(0.02f);
        material.specular = glm::vec3(0.15f);
        material.shininess = 16.0f;
        tree->setMaterial(material);
    }
    scene->add(tree);
    insertedObjects.emplace_back(scene, tree);
    std::cout << "Inserted tree, id " << tree->getId() << std::endl;
}

void Application::addMovingObject(Scene* scene, const glm::vec3& a, const glm::vec3& b) {
    const float radius = scene == terrainScene ? 1.0f : 0.3f;
    const float speed = scene == terrainScene ? 6.0f : 2.0f;
    const glm::vec3 lift(0.0f, radius, 0.0f);

    TransformationComposite* transformation = make<TransformationComposite>();
    transformation->addDynamic(make<LineMove>(a + lift, b + lift, speed));
    transformation->addStatic(Scale(radius));

    DrawableObject* sphereObject = createObject(sphereModel, phongProgram, transformation);
    sphereObject->setColor(glm::vec3(1.0f, 0.5f, 0.1f));
    Material material; 
    material.ambient = glm::vec3(0.6f);
    material.diffuse = glm::vec3(0.6f);
    material.specular = glm::vec3(0.5f);
    material.shininess = 32.0f;
    sphereObject->setMaterial(material);
    sphereObject->setKind(DrawableObject::Kind::Moving);
    scene->add(sphereObject);
    insertedObjects.emplace_back(scene, sphereObject);
    std::cout << "Moving object " << sphereObject->getId() << " created" << std::endl;
}

void Application::addCollectibles(Scene* scene) {
    std::mt19937 generator(7);
    std::uniform_real_distribution<float> x(-9.0f, 9.0f);
    std::uniform_real_distribution<float> z(4.0f, 15.0f);

    const int COUNT = 10;
    for (int i = 0; i < COUNT; ++i) {
        TransformationComposite* transformation = makeComposite(scene);
        transformation->addStatic(Translate(glm::vec3(x(generator), 0.35f, z(generator))));
        transformation->addStatic(Scale(0.25f));
        DrawableObject* coin = createObject(sphereModel, constantProgram, transformation);
        coin->setColor(glm::vec3(1.0f, 0.8f, 0.1f));
        coin->setKind(DrawableObject::Kind::Collectible);
        scene->add(coin);
    }
    collectibleTotal += COUNT;
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (activeScene < scenes.size()) {
            scenes[activeScene]->draw();
        }

        if (clickPending) {
            clickPending = false;
            handleClick();
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

    const bool sprint = pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    const float cameraStep = deltaTime * (sprint ? 3.0f : 1.0f);

    Camera* camera = scene->getCamera();
    if (pressed(GLFW_KEY_W)) camera->moveForward(cameraStep);
    if (pressed(GLFW_KEY_S)) camera->moveBackward(cameraStep);
    if (pressed(GLFW_KEY_A)) camera->moveLeft(cameraStep);
    if (pressed(GLFW_KEY_D)) camera->moveRight(cameraStep);
    if (pressed(GLFW_KEY_E)) camera->moveUp(cameraStep);
    if (pressed(GLFW_KEY_Q)) camera->moveDown(cameraStep);

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
