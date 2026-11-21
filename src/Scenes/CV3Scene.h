// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"
#include "Lighting/Material.h"

class CV3Scene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

private:

    void buildTerrain();
    void buildRocks();
    void buildTrees();
    void buildBushes();

    Material WaterM;
	Material OrganicM;
	Material WoodM;
	Material MushroomM;

    ShaderProgram* shader = nullptr;

    float terrainHeight(float x, float z) const;
    bool isInCamp(float x, float z) const;
    bool isInWater(float x, float z) const;
    Light* campfireLight;
};
