// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

// Cista scena pripravena na pridavani funkci.
// V create() vynuluje vychozi zaklad ze Scene::build() (kamera, podlaha, skybox).
class TestScene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;
};
