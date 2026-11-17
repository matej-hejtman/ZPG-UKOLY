// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

#include <vector>

class Texture;
struct Material;

class ShowcaseScene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

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

    void buildSolarSystem();
    void buildMaterialRing();
    void buildPendulumWave();
    void buildHelix();
    void buildEqualizer();
    void buildDrones();
    void buildLights();
    void buildHud();

    DrawableObject* spawn(Transformation* transformation, const Material& material, const glm::vec3& color = glm::vec3(1.0f), bool textured = false, bool decorative = false);

    Model* cubeModel = nullptr;
    ShaderProgram* litShader = nullptr;
    Texture* defaultTexture = nullptr;

    std::vector<Column> columns;
    std::vector<Bobber> bobbers;
    std::vector<Light*> ringLights;
    std::vector<glm::vec3> ringColors;
    std::vector<glm::vec3> ringPositions;
    std::vector<Light*> orbitLights;
    std::vector<DrawableObject*> orbitMarkers;
    std::vector<DrawableObject*> hudSwatches;
    std::vector<DrawableObject*> ringTrims;

    Light* spotlight = nullptr;
    Light* equalizerLight = nullptr;
    Scale* sunPulse = nullptr;
};
