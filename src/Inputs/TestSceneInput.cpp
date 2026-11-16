// Author: Matěj Hejtman, login: HEJ0094
#include "Inputs/TestSceneInput.h"
#include "Scenes/TestScene.h"

#include <GLFW/glfw3.h>

TestSceneInput::TestSceneInput(TestScene& scene) : SceneInput(scene), testScene(scene) {}

void TestSceneInput::onKey(GLFWwindow* window, int key, int action, int mods) {
    // sem pujdou klavesy test sceny (prepinani shaderu, svetel...)

    SceneInput::onKey(window, key, action, mods);
}

void TestSceneInput::onHudClick(DrawableObject* object) {
    if (object == testScene.getLightButton()) {
        testScene.toggleLight();
    }
}

void TestSceneInput::onGroundClick(DrawableObject* ground, const glm::vec3& position) {
    (void)ground;
    testScene.insertSphere(position);
}

void TestSceneInput::onObjectClick(DrawableObject* object, const glm::vec3& position) {
    (void)position;
    testScene.toggleHighlight(object);
}
