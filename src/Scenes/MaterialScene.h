// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

class MaterialScene : public Scene {
public:
    using Scene::Scene;

    void create() override;
};
