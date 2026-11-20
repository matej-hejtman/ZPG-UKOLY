// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

class TestScene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

private:
    void buildMetalMaterials();
    void buildPolishedMetalMaterials();
    void buildPreciousMaterials();
    void buildPlacticMaterials();
    void buildRubberMaterials();

    void buildTransformations();
    void buildAxes(const glm::vec3& origin);

    ShaderProgram* shader = nullptr;
};
