// Author: Matěj Hejtman, login: HEJ0094
#include "Inputs/TestSceneInput.h"
#include "Scenes/TestScene.h"

#include <GLFW/glfw3.h>

TestSceneInput::TestSceneInput(TestScene& scene) : SceneInput(scene), testScene(scene) {}

void TestSceneInput::onKey(GLFWwindow* window, int key, int action, int mods) {
    SceneInput::onKey(window, key, action, mods);
}
