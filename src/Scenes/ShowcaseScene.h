// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

#include <random>
#include <vector>

class DynamicRotate;
class GrowScale;
class LineMove;
class Oscillate;
class ShearWobble;
class Texture;
struct Material;

class ShowcaseScene : public Scene {
public:
    enum class Environment {
        Night,
        Space,
        Fog
    };

    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

    void spawnCube(const glm::vec3& worldPosition);
    void clearSpawned();
    void interact(DrawableObject* object);
    void hudClick(DrawableObject* object);
    void replayIntro();
    bool toggleFlashlight();
    bool toggleChase();
    bool toggleHud();

    const char* cycleEnvironment();
    bool toggleMiniature();
    void nudgeWorld(float spinDeg, float lift);
    void resetWorld();
    const char* cyclePendulums();
    void startCollectibles();
    void supernova();
    DrawableObject* selectNext();
    void printSelection();
    glm::vec3 toWorld(const glm::vec3& local);

    static const glm::vec3 SUN_POSITION;

private:
    struct Column {
        DrawableObject* object;
        Scale* height;
        float distance;
        float x;
    };

    struct Bobber {
        Translate* offset;
        float phase;
    };

    struct Spawned {
        DrawableObject* object;
        Light* light;
        Translate* position;
        Scale* squash;
        glm::vec3 base;
        float velocity;
        float squashAmount;
        float size;
        float dying;
    };

    struct Flash {
        DrawableObject* object;
        glm::vec3 baseColor;
        float start;
    };

    struct Drone {
        LineMove* path;
        Rotate* roll;
        Light* light;
        DrawableObject* glow;
        Translate* marker;
        glm::vec3 color;
        bool returning;
        float rollTime;
        float boost;
    };

    struct Pendulum {
        Oscillate* swing;
        float frequency;
    };

    struct Collectible {
        DrawableObject* object;
        Light* light;
        GrowScale* grow;
        Translate* bob;
        float phase;
        int state;
    };

    void buildSolarSystem();
    void buildMaterialRing();
    void buildPendulumWave();
    void buildHelix();
    void buildEqualizer();
    void buildDrones();
    void buildLights();
    void buildGate();
    void buildSky();
    void buildHud();

    void applyEnvironment();
    void updateWorld(float deltaTime);
    void updateSpawned(float deltaTime);
    void updateDrones(float deltaTime);
    void updateCollectibles(float deltaTime);
    void updateSun();
    void updateHud();
    void updateFlashes();
    void flash(DrawableObject* object);
    void forget(DrawableObject* object);
    void kill(Spawned& cube);
    void removeCollectibles();

    DrawableObject* spawn(Transformation* transformation, const Material& material, const glm::vec3& color = glm::vec3(1.0f), bool textured = false, bool decorative = false);
    DrawableObject* hudSquare(HudAnchor anchor, const glm::vec2& offset, float scale);
    GrowScale* intro(float durationSeconds, const glm::vec3& from = glm::vec3(0.001f), const glm::vec3& pivot = glm::vec3(0.0f));

    Model* cubeModel = nullptr;
    ShaderProgram* litShader = nullptr;
    Texture* defaultTexture = nullptr;
    Texture* crispTexture = nullptr;

    std::vector<Column> columns;
    std::vector<Bobber> bobbers;
    std::vector<Light*> ringLights;
    std::vector<glm::vec3> ringColors;
    std::vector<glm::vec3> ringPositions;
    std::vector<DrawableObject*> ringCubes;
    std::vector<DynamicRotate*> ringSpins;
    std::vector<float> ringSpinSpeeds;
    std::vector<float> ringBoosts;
    std::vector<DrawableObject*> ringTrims;
    std::vector<Light*> orbitLights;
    std::vector<DrawableObject*> orbitMarkers;
    std::vector<GrowScale*> introAnimations;
    std::vector<Spawned> spawned;
    std::vector<Flash> flashes;
    std::vector<Drone> drones;
    std::vector<Pendulum> pendulums;
    std::vector<Collectible> collectibles;
    std::vector<DrawableObject*> sunCubes;
    std::vector<DrawableObject*> gateParts;

    std::vector<DrawableObject*> hudSwatches;
    std::vector<DrawableObject*> hudEnvironment;
    std::vector<DrawableObject*> hudCollected;
    std::vector<DrawableObject*> hudTelemetry;
    std::vector<DrawableObject*> hudSpeed;
    DrawableObject* hudCrosshair = nullptr;

    Light* spotlight = nullptr;
    Light* flashlight = nullptr;
    Light* equalizerLight = nullptr;
    Light* sunLight = nullptr;
    Light* moonLight = nullptr;
    Scale* sunPulse = nullptr;
    GrowScale* sunBurst = nullptr;
    ShearWobble* gateWobble = nullptr;
    DrawableObject* gateNeon = nullptr;
    DrawableObject* dome = nullptr;
    DrawableObject* selected = nullptr;

    Environment environment = Environment::Night;
    int spotlightFocus = -1;
    int pendulumMode = 0;
    int sunPhase = 0;
    int collectedCount = 0;
    int collectTotal = 0;
    bool chaseEnabled = true;
    bool hudVisible = true;
    bool miniature = false;
    float worldZoom = 1.0f;
    float worldZoomTarget = 1.0f;
    float worldLift = 0.0f;
    float worldLiftTarget = 0.0f;
    float worldSpin = 0.0f;
    float worldSpinTarget = 0.0f;
    float celebrateUntil = -1.0f;
    float realTime = 0.0f;
    std::mt19937 random{ 20251117u };
};
