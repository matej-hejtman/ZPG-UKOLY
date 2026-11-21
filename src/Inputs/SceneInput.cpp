// Author: Matěj Hejtman, login: HEJ0094
#include "Inputs/SceneInput.h"
#include "Core/Scene.h"
#include "Rendering/DrawableObject.h"

#include <GLFW/glfw3.h>

#include <algorithm>

static const float FOV_STEP = 15.0f;
static const float FOV_MIN = 15.0f;
static const float FOV_MAX = 120.0f;

SceneInput::SceneInput(Scene& scene) : scene(scene) {}

void SceneInput::onKey(GLFWwindow* window, int key, int action, int mods) {
    (void)window; (void)key; (void)action; (void)mods;
}

void SceneInput::onMouseButton(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &clickX, &clickY);
        clickPending = true;
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        scene.getCamera()->resetFov();
    }
}

void SceneInput::onMouseMove(GLFWwindow* window, double dx, double dy) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        scene.getCamera()->rotate(static_cast<float>(dx), static_cast<float>(dy));
    }
}

void SceneInput::onScroll(GLFWwindow* window, double dx, double dy) {
    (void)window; (void)dx;

    Camera* camera = scene.getCamera();
    const float fov = camera->getFov() - static_cast<float>(dy) * FOV_STEP;
    camera->setFov(std::clamp(fov, FOV_MIN, FOV_MAX));
}

void SceneInput::update(GLFWwindow* window, float deltaTime) {
    auto pressed = [window](int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    const bool sprint = pressed(GLFW_KEY_LEFT_SHIFT) || pressed(GLFW_KEY_RIGHT_SHIFT);
    const float step = deltaTime * (sprint ? SPRINT_MULTIPLIER : 1.0f);

    Camera* camera = scene.getCamera();
    if (pressed(GLFW_KEY_W)) camera->moveForward(step);
    if (pressed(GLFW_KEY_S)) camera->moveBackward(step);
    if (pressed(GLFW_KEY_A)) camera->moveLeft(step);
    if (pressed(GLFW_KEY_D)) camera->moveRight(step);
    if (pressed(GLFW_KEY_E)) camera->moveUp(step);
    if (pressed(GLFW_KEY_Q)) camera->moveDown(step);
}

void SceneInput::processPendingClick(GLFWwindow* window) {
    if (!clickPending) {
        return;
    }
    clickPending = false;

    int windowWidth = 0, windowHeight = 0, width = 0, height = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glfwGetFramebufferSize(window, &width, &height);
    if (windowWidth <= 0 || windowHeight <= 0) {
        return;
    }

    const int x = static_cast<int>(clickX * width / windowWidth);
    const int y = static_cast<int>(clickY * height / windowHeight);

    const Scene::PickResult hit = scene.pick(x, y);

    if (!hit.object) {
        onBackgroundClick();
    } else if (hit.isHud) {
        onHudClick(hit.object);
    } else if (hit.object->getKind() == DrawableObject::Kind::Ground) {
        onGroundClick(hit.object, hit.position);
    } else {
        onObjectClick(hit.object, hit.position);
    }
}

void SceneInput::onHudClick(DrawableObject* object) {
    (void)object;
}

void SceneInput::onGroundClick(DrawableObject* ground, const glm::vec3& position) {
    (void)ground; (void)position;
}

void SceneInput::onObjectClick(DrawableObject* object, const glm::vec3& position) {
    (void)object; (void)position;
}

void SceneInput::onBackgroundClick() {}
