// Author: Matěj Hejtman, login : HEJ0094
#include "Inputs/CV4Input.h"
#include "Scenes/CV4Scene.h"

#include <GLFW/glfw3.h>

CV4Input::CV4Input(CV4Scene& scene) : SceneInput(scene), cv4Scene(scene) {
    SPRINT_MULTIPLIER = 20.f;
}

void CV4Input::onKey(GLFWwindow* window, int key, int action, int mods) {
    SceneInput::onKey(window, key, action, mods);

    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_SPACE:
        ringsVisible = !ringsVisible;
        cv4Scene.setRingsVisible(ringsVisible);

        break;

    default:
        break;
    }
}
