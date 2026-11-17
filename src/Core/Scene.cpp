// Author: Matěj Hejtman, login: HEJ0094
#include "Core/Scene.h"
#include "Rendering/DrawableObject.h"
#include "Core/Resources.h"
#include "Lighting/Material.h"
#include "Rendering/ModelLoader.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/Skybox.h"

#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <algorithm>
#include <filesystem>
#include <iostream>

// ======================================
// Vychozi zaklad kazde sceny
// ======================================

// vychozi textura podlahy a skybox jsou Texture::DEFAULT_PATH a CubeMap::DEFAULT_DIRECTORY
static const glm::vec3 DEFAULT_CAMERA_POSITION = glm::vec3(0.0f, 1.7f, 0.0f);

Scene::Scene(Resources& resources) : resources(resources) {
    base.addDynamic(&controlMove);
    base.addDynamic(&controlSpin);
    base.addDynamic(&controlZoom);
    lightBuffer = resources.getLightBuffer();
}

Scene::~Scene() = default;

void Scene::build() {
    createDefaults();
    create();
}

void Scene::createDefaults() {
    // kamera uprostred, pohled podel -Z
    camera.setPosition(DEFAULT_CAMERA_POSITION, 90.0f, -90.0f);

    // podlaha 20x20, Kind::Ground -> da se na ni klikem vkladat
    Material floorMaterial;
    floorMaterial.ambient = glm::vec3(0.25f);
    floorMaterial.specular = glm::vec3(0.3f);

    ground = createObject(resources.getModel("floor"), resources.getShader("lit.vert", "phong.frag"), makeComposite());
    ground->setMaterial(floorMaterial);
    ground->setTexture(resources.getTexture(Texture::DEFAULT_PATH));
    ground->setKind(DrawableObject::Kind::Ground);

    setSkybox(resources.getSkybox(CubeMap::DEFAULT_DIRECTORY));

    // HUD: zatim prazdny, prvky se pridavaji pres createHudObject()
    hudProgram = resources.getShader("hud.vert", "hud.frag");
}

void Scene::create() {}

void Scene::tick(float deltaTime) {
    clock.advance(deltaTime);
    update(deltaTime);
}

void Scene::update(float deltaTime) {
    (void)deltaTime;
}

SceneClock& Scene::getClock() {
    return clock;
}

void Scene::activate(int width, int height) {
    resize(width, height);
    camera.notifyAll();
}

void Scene::resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;     // minimalizovane okno
    }

    viewportWidth = width;
    viewportHeight = height;
    hudAspect = static_cast<float>(width) / static_cast<float>(height);

    // kamera je subject: setAspect -> notifyAll -> shadery dostanou novou projekci
    camera.setAspect(width, height);
}

// ======================================
// Objekty
// ======================================

DrawableObject* Scene::createObject(Model* model, ShaderProgram* program, Transformation* transformation) {
    ownedObjects.push_back(std::make_unique<DrawableObject>(model, program, transformation));
    DrawableObject* object = ownedObjects.back().get();
    registerProgram(program);
    add(object);
    return object;
}

std::vector<DrawableObject*> Scene::createObjModel(const std::string& path, ShaderProgram* program, Transformation* transformation, bool mergeColors) {
    const LoadedModel& loaded = resources.getObjModel(path, mergeColors);
    std::vector<DrawableObject*> created;

    for (const ModelPart& part : loaded.parts) {
        DrawableObject* object = createObject(part.model.get(), program, transformation);
        object->setMaterial(part.material);

        if (!part.texturePath.empty() && std::filesystem::exists(part.texturePath)) {
            object->setTexture(resources.getTexture(part.texturePath));
        } else {
            if (!part.texturePath.empty()) {
                std::cerr << "WARNING: texture not found: " << part.texturePath << " (" << path << ")" << std::endl;
            }
            object->setColor(part.diffuseColor);
        }

        created.push_back(object);
    }

    return created;
}

TransformationComposite* Scene::makeComposite() {
    TransformationComposite* composite = make<TransformationComposite>();
    composite->addDynamic(&base);
    return composite;
}

void Scene::registerProgram(ShaderProgram* program) {
    if (!program || std::find(programs.begin(), programs.end(), program) != programs.end()) {
        return;
    }

    programs.push_back(program);
    camera.attach(program);
    program->onCameraChanged(&camera);
}

unsigned Scene::allocateId() {
    if (!freeIds.empty()) {
        const unsigned id = freeIds.back();
        freeIds.pop_back();
        return id;
    }

    if (nextId <= 255) {
        return nextId++;
    }

    std::cerr << "WARNING: scene has more than 255 objects, new object is not selectable" << std::endl;
    return 0;
}

void Scene::releaseId(DrawableObject* object) {
    if (object->getId() != 0) {
        freeIds.push_back(object->getId());
    }
    object->setId(0);
}

void Scene::add(DrawableObject* object) {
    object->setId(allocateId());
    objects.push_back(object);
}

void Scene::addStatic(DrawableObject* object) {
    staticObjects.push_back(object);
}

bool Scene::remove(DrawableObject* object) {
    for (std::vector<DrawableObject*>* list : { &objects, &hudObjects, &staticObjects }) {
        auto it = std::find(list->begin(), list->end(), object);
        if (it != list->end()) {
            list->erase(it);
            releaseId(object);

            if (object == ground) {
                ground = nullptr;
            }
            return true;
        }
    }
    return false;
}

DrawableObject* Scene::getObjectById(unsigned id) const {
    if (id == 0) {
        return nullptr;
    }

    for (const std::vector<DrawableObject*>* list : { &objects, &hudObjects }) {
        for (DrawableObject* object : *list) {
            if (object->getId() == id) {
                return object;
            }
        }
    }
    return nullptr;
}

// ======================================
// HUD
// ======================================

DrawableObject* Scene::createHudObject(Model* model, HudAnchor anchor, const glm::vec2& offset, float scale) {
    // HUD transformace NENI napojena na base -> neovlivni ji posun / rotace / zoom sceny
    HudPlacement* placement = make<HudPlacement>(&hudAspect, anchor, offset, scale);

    ownedObjects.push_back(std::make_unique<DrawableObject>(model, hudProgram, placement));
    DrawableObject* object = ownedObjects.back().get();
    object->setId(allocateId());
    hudObjects.push_back(object);
    return object;
}

bool Scene::isHud(const DrawableObject* object) const {
    return std::find(hudObjects.begin(), hudObjects.end(), object) != hudObjects.end();
}

// ======================================
// Picking
// ======================================

Scene::PickResult Scene::pick(int x, int y) const {
    PickResult result;

    if (x < 0 || y < 0 || x >= viewportWidth || y >= viewportHeight) {
        return result;
    }

    const int glY = viewportHeight - 1 - y;    // OpenGL ma pocatek vlevo dole

    GLfloat depth = 1.0f;
    GLuint index = 0;
    glReadPixels(x, glY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    glReadPixels(x, glY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

    result.object = getObjectById(index);
    result.isHud = result.object && isHud(result.object);
    result.hasPoint = !result.isHud && depth < 1.0f;

    if (result.hasPoint) {
        const glm::vec3 screen(static_cast<float>(x), static_cast<float>(glY), depth);
        const glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight));
        result.position = glm::unProject(screen, camera.getViewMatrix(), camera.getProjectionMatrix(), viewport);
    }

    return result;
}

// ======================================
// Vykresleni
// ======================================

void Scene::draw() const {
    if (skybox) {
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        skybox->draw();
    }

    // svetla do UBO: kazde si spocita pozici a smer ze sve transformace (Light::toData)
    std::vector<LightData> data;
    for (const auto& light : lights) {
        if (!light->isEnabled()) {
            continue;
        }

        if (static_cast<int>(data.size()) >= MAX_LIGHTS) {
            static bool warned = false;
            if (!warned) {
                std::cerr << "WARNING: more than " << MAX_LIGHTS << " enabled lights, the rest is ignored" << std::endl;
                warned = true;
            }
            break;
        }

        data.push_back(light->toData());
    }

    if (lightBuffer) {
        lightBuffer->update(data, ambientLight);
    }

    for (DrawableObject* object : staticObjects) {
        object->draw();
    }

    for (DrawableObject* object : objects) {
        object->draw();
    }

    // HUD navrch: vlastni ortho projekce, bez kamery a bez hloubky
    if (hudProgram && !hudObjects.empty()) {
        hudProgram->use();
        hudProgram->setUniform("projectionMatrix", glm::ortho(-hudAspect, hudAspect, -1.0f, 1.0f, -1.0f, 1.0f));

        glDisable(GL_DEPTH_TEST);
        for (DrawableObject* object : hudObjects) {
            object->draw();
        }
        glEnable(GL_DEPTH_TEST);
    }

    ShaderProgram::unbind();
}

// ======================================
// Gettery / settery
// ======================================

DrawableObject* Scene::getGround() const {
    return ground;
}

void Scene::removeGround() {
    if (ground) {
        remove(ground);
    }
}

Camera* Scene::getCamera() {
    return &camera;
}

Light* Scene::addLight(bool attachToBase) {
    lights.push_back(std::make_unique<Light>());
    Light* light = lights.back().get();

    if (attachToBase) {
        light->getTransformation().addDynamic(&base);   // jako makeComposite()
    }
    return light;
}

bool Scene::removeLight(Light* light) {
    for (auto it = lights.begin(); it != lights.end(); ++it) {
        if (it->get() == light) {
            lights.erase(it);
            return true;
        }
    }
    return false;
}

const std::vector<std::unique_ptr<Light>>& Scene::getLights() const {
    return lights;
}

void Scene::setAmbientLight(const glm::vec3& value) {
    ambientLight = value;
}

const glm::vec3& Scene::getAmbientLight() const {
    return ambientLight;
}

void Scene::setSkybox(Skybox* value) {
    skybox = value;
    if (skybox) {
        registerProgram(skybox->getProgram());
    }
}

void Scene::setClearColor(const glm::vec3& value) {
    clearColor = value;
}

const glm::vec3& Scene::getClearColor() const {
    return clearColor;
}

TransformationComposite* Scene::getBase() {
    return &base;
}

Translate& Scene::getMove() {
    return controlMove;
}

Rotate& Scene::getSpin() {
    return controlSpin;
}

Scale& Scene::getZoom() {
    return controlZoom;
}
