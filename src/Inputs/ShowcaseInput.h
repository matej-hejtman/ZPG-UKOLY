// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Inputs/SceneInput.h"

#include <glm/glm.hpp>

class ShowcaseScene;

class ShowcaseInput : public SceneInput {
public:
    explicit ShowcaseInput(ShowcaseScene& scene);

    void onKey(GLFWwindow* window, int key, int action, int mods) override;
    void onMouseMove(GLFWwindow* window, double dx, double dy) override;
    void update(GLFWwindow* window, float deltaTime) override;

protected:
    void onHudClick(DrawableObject* object) override;
    void onGroundClick(DrawableObject* ground, const glm::vec3& position) override;
    void onObjectClick(DrawableObject* object, const glm::vec3& position) override;

private:
    enum class Autopilot {
        None,
        Flight,
        Orbit,
        Vertigo
    };

    struct Shot {
        const char* name;
        glm::vec3 eye;
        glm::vec3 lookAt;
    };

    void flyTo(const Shot& shot);
    void startOrbit();
    void startVertigo();
    void stopAutopilot();
    void aim(const glm::vec3& eye, const glm::vec3& lookAt);
    void changeSpeed(float factor);
    void togglePause();
    static void printHelp();

    ShowcaseScene& showcase;

    Autopilot autopilot = Autopilot::None;
    glm::vec3 flightFromEye = glm::vec3(0.0f);
    glm::vec3 flightFromLook = glm::vec3(0.0f);
    glm::vec3 flightToEye = glm::vec3(0.0f);
    glm::vec3 flightToLook = glm::vec3(0.0f);
    float flightProgress = 0.0f;
    float orbitAngle = 0.0f;
    float orbitRadius = 0.0f;
    float orbitHeight = 0.0f;

    glm::vec3 vertigoTarget = glm::vec3(0.0f);
    glm::vec3 vertigoDirection = glm::vec3(0.0f);
    float vertigoDistance = 0.0f;
    float vertigoWidth = 0.0f;
    float vertigoFov = 60.0f;
    float vertigoProgress = 0.0f;

    bool helpShown = false;
};
