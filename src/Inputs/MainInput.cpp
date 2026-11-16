// Author: Matěj Hejtman, login: HEJ0094
#include "Inputs/MainInput.h"
#include "Application.h"

#include <GLFW/glfw3.h>

MainInput::MainInput(Application& app) : app(app) {}

bool MainInput::onKey(int key, int action, int mods) {
    if (action != GLFW_PRESS) {
        return false;
    }

    switch (key) {
    case GLFW_KEY_ESCAPE:
        app.close();
        return true;

    case GLFW_KEY_TAB:
        app.switchScene((mods & GLFW_MOD_SHIFT) ? -1 : 1);
        return true;

    case GLFW_KEY_F11:
        app.toggleFullscreen();
        return true;

    default:
        return false;
    }
}
