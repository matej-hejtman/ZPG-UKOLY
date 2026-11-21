// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;
class DrawableObject;
class Scene;

class SceneInput {
public:
    explicit SceneInput(Scene& scene);
    virtual ~SceneInput() = default;
    SceneInput(const SceneInput&) = delete;
    SceneInput& operator=(const SceneInput&) = delete;

    virtual void onKey(GLFWwindow* window, int key, int action, int mods);
    virtual void onMouseButton(GLFWwindow* window, int button, int action, int mods);
    virtual void onMouseMove(GLFWwindow* window, double dx, double dy);
    virtual void onScroll(GLFWwindow* window, double dx, double dy);

    virtual void update(GLFWwindow* window, float deltaTime);

    void processPendingClick(GLFWwindow* window);

protected:
    virtual void onHudClick(DrawableObject* object);
    virtual void onGroundClick(DrawableObject* ground, const glm::vec3& position);
    virtual void onObjectClick(DrawableObject* object, const glm::vec3& position);
    virtual void onBackgroundClick();

    float SPRINT_MULTIPLIER = 3.0f;

    Scene& scene;

private:
    bool clickPending = false;
    double clickX = 0.0;
    double clickY = 0.0;
};
