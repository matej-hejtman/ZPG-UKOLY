// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Camera/Camera.h"
#include "Core/SceneClock.h"
#include "Lighting/Light.h"
#include "Transformations/HudPlacement.h"
#include "Transformations/Rotate.h"
#include "Transformations/Scale.h"
#include "Transformations/TransformationComposite.h"
#include "Transformations/Translate.h"
#include "Lighting/LightBuffer.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

class DrawableObject;
class Model;
class Resources;
class ShaderProgram;
class Skybox;

class Scene {
public:
    struct PickResult {
        DrawableObject* object = nullptr;
        bool isHud = false;
        bool hasPoint = false;
        glm::vec3 position = glm::vec3(0.0f);
    };

    explicit Scene(Resources& resources);
    virtual ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void build();

    virtual void create();

    void tick(float deltaTime);

    virtual void update(float deltaTime);

    SceneClock& getClock();

    void activate(int width, int height);

    void resize(int width, int height);

    DrawableObject* createObject(Model* model, ShaderProgram* program, Transformation* transformation);

    std::vector<DrawableObject*> createObjModel(const std::string& path, ShaderProgram* program, Transformation* transformation, bool mergeColors = false);

    TransformationComposite* makeComposite();

    template <class T, class... Args>
    T* make(Args&&... args) {
        auto owned = std::make_unique<T>(std::forward<Args>(args)...);
        T* pointer = owned.get();
        transformations.push_back(std::move(owned));
        return pointer;
    }

    void registerProgram(ShaderProgram* program);

    DrawableObject* createStaticObject(Model* model, ShaderProgram* program, Transformation* transformation);

    bool remove(DrawableObject* object);
    DrawableObject* getObjectById(unsigned id) const;

    DrawableObject* createHudObject(Model* model, HudAnchor anchor, const glm::vec2& offset, float scale);
    bool isHud(const DrawableObject* object) const;

    PickResult pick(int x, int y) const;


    DrawableObject* getGround() const;
    void removeGround();


    Camera* getCamera();

    Light* addLight(bool attachToBase = true);
    bool removeLight(Light* light);
    const std::vector<std::unique_ptr<Light>>& getLights() const;

    void setAmbientLight(const glm::vec3& value);
    const glm::vec3& getAmbientLight() const;

    void setSkybox(Skybox* value);
    void setClearColor(const glm::vec3& value);
    const glm::vec3& getClearColor() const;

    TransformationComposite* getBase();
    Translate& getMove();
    Rotate& getSpin();
    Scale& getZoom();

    void draw() const;

protected:
    Resources& resources;

private:
    void createDefaults();
    unsigned allocateId();
    void releaseId(DrawableObject* object);

    std::vector<std::unique_ptr<DrawableObject>> ownedObjects;
    std::vector<std::unique_ptr<Transformation>> transformations;

    std::vector<DrawableObject*> objects;
    std::vector<DrawableObject*> staticObjects;
    std::vector<DrawableObject*> hudObjects;
    unsigned nextId = 1;
    std::vector<unsigned> freeIds;

    Camera camera;
    SceneClock clock;
    std::vector<std::unique_ptr<Light>> lights;
    std::vector<ShaderProgram*> programs;
    glm::vec3 ambientLight = glm::vec3(1.0f);
    glm::vec3 clearColor = glm::vec3(0.0f);
    Skybox* skybox = nullptr;

    ShaderProgram* hudProgram = nullptr;
    float hudAspect = 4.0f / 3.0f;
    int viewportWidth = 800;
    int viewportHeight = 600;

    DrawableObject* ground = nullptr;

    Translate controlMove{glm::vec3(0.0f)};
    Rotate controlSpin{0.0f, glm::vec3(0.0f, 1.0f, 0.0f)};
    Scale controlZoom{1.0f};
    TransformationComposite base;

	LightBuffer* lightBuffer = nullptr;
};
