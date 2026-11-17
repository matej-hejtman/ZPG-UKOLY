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

// Engine kontejner sceny: objekty, svetla, kamera, HUD.
// Konkretni sceny (src/Scenes/) z ni dedi a v create() si postavi obsah.
// Scena VLASTNI jen svoje objekty, transformace a svetla.
// Sdilene zdroje (shadery, modely, textury) patri do Resources.
//
// Kazda scena ma po build() vychozi zaklad (createDefaults):
//   - kamera uprostred sceny                                           -> getCamera()->setPosition(...)
//   - otexturovana podlaha (Kind::Ground, da se na ni klikem vkladat)  -> getGround() / removeGround()
//   - skybox                                                           -> setSkybox(jiny) / setSkybox(nullptr)
//   - prazdny HUD                                                      -> createHudObject(...)
// Konkretni scena to v create() muze zmenit nebo smazat.
class Scene {
public:
    // vysledek kliknuti (Scene::pick)
    struct PickResult {
        DrawableObject* object = nullptr;   // na co se kliklo (nullptr = pozadi / skybox)
        bool isHud = false;                 // object je HUD prvek
        bool hasPoint = false;              // kliklo se na geometrii sceny (ne na nebe)
        glm::vec3 position = glm::vec3(0.0f); // bod ve svete (unProject), plati jen kdyz hasPoint
    };

    explicit Scene(Resources& resources);
    virtual ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    // vola Application: vychozi zaklad + create() konkretni sceny
    void build();

    virtual void create();

    // vola Application jednou za snimek: posune hodiny sceny a zavola update()
    void tick(float deltaTime);

    // logika konkretni sceny; deltaTime je skutecny cas snimku,
    // cas animaci (po pauze / zrychleni) je v getClock().getDelta()
    virtual void update(float deltaTime);

    // cas sceny pro animovane transformace, napr. make<DynamicRotate>(getClock(), 90.0f, osa)
    SceneClock& getClock();

    // vola Application pri prepnuti na tuto scenu
    void activate(int width, int height);

    // vola Application pri zmene velikosti okna (kamera + HUD)
    void resize(int width, int height);

    // ---------- objekty ----------

    // vytvori objekt, scena ho vlastni a rovnou ho prida (vybiratelny pres stencil)
    DrawableObject* createObject(Model* model, ShaderProgram* program, Transformation* transformation);

    // OBJ model ze sdilenych Resources: kazdy dil = jeden DrawableObject se spolecnou transformaci.
    // Dil s texturou dostane texturu, dil bez textury (nebo s chybejici) barvu z .mtl.
    std::vector<DrawableObject*> createObjModel(const std::string& path, ShaderProgram* program, Transformation* transformation, bool mergeColors = false);

    // composite uz napojeny na base sceny (posun / rotace / zoom cele sceny)
    TransformationComposite* makeComposite();

    template <class T, class... Args>
    T* make(Args&&... args) {
        auto owned = std::make_unique<T>(std::forward<Args>(args)...);
        T* pointer = owned.get();
        transformations.push_back(std::move(owned));
        return pointer;
    }

    // shader pripoji na kameru (view/projection) a posila mu svetla
    // createObject to dela sam, rucne jen kdyz objektu menis shader za behu
    void registerProgram(ShaderProgram* program);

    void add(DrawableObject* object);
    void addStatic(DrawableObject* object);
    bool remove(DrawableObject* object);   // objekt i HUD prvek
    DrawableObject* getObjectById(unsigned id) const;

    // ---------- HUD ----------

    // staticka vrstva pred kamerou: neovlivni ji kamera ani posun sceny,
    // kresli se posledni (vzdy navrchu) a da se na ni kliknout
    DrawableObject* createHudObject(Model* model, HudAnchor anchor, const glm::vec2& offset, float scale);
    bool isHud(const DrawableObject* object) const;

    // ---------- picking ----------

    // x, y = pixel ve framebufferu, pocatek vlevo nahore (jako u kurzoru)
    // volat po draw() a pred swapem bufferu
    PickResult pick(int x, int y) const;

    // ---------- vychozi zaklad ----------

    DrawableObject* getGround() const;
    void removeGround();

    // ---------- ostatni ----------

    Camera* getCamera();

    // svetlo s vlastni transformaci, ve vychozim stavu napojenou na base sceny.
    // Pripnuti: light->getTransformation().addDynamic(make<DynamicRotate>(...)) apod.
    // attachToBase = false, kdyz ho pripinas k transformaci objektu (ta uz base obsahuje).
    Light* addLight(bool attachToBase = true);
    bool removeLight(Light* light);
    const std::vector<std::unique_ptr<Light>>& getLights() const;

    // ambientni svetlo sceny (nasobi material.ambient), napr. tmavsi a modrejsi pro noc
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
