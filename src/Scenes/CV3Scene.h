// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

class CV3Scene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

};
