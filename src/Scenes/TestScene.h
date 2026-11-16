// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

class DrawableObject;

// God scena: na dummy objektech ukazuje vse, co engine umi.
// Vychozi zaklad (kamera, podlaha, skybox, HUD podpis) dela Scene::build().
class TestScene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

    // pro TestSceneInput
    DrawableObject* getLightButton() const;
    void toggleLight();
    void insertSphere(const glm::vec3& groundPosition);
    void toggleHighlight(DrawableObject* object);

private:
    DrawableObject* sphere = nullptr;
    Light* light = nullptr;
    DrawableObject* lightButton = nullptr;
    DrawableObject* highlighted = nullptr;
    glm::vec3 highlightedColor = glm::vec3(1.0f);
};
