// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Inputs/SceneInput.h"

class TestScene;

class TestSceneInput : public SceneInput {
public:
    explicit TestSceneInput(TestScene& scene);

    void onKey(GLFWwindow* window, int key, int action, int mods) override;

private:
    TestScene& testScene;
};
