// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;
class DrawableObject;
class Scene;

// Zaklad inputu kazde sceny:
//   W/S/A/D/Q/E ... pohyb kamery, Shift ... rychleji
//   prave tlacitko + mys ... rozhlizeni
//   kolecko ... FOV po 15 stupnich (kamera jako observer to propise do shaderu)
//   leve tlacitko ... kliknuti -> onHudClick / onGroundClick / onObjectClick / onBackgroundClick
//
// Input konkretni sceny z nej dedi a prepise, co potrebuje.
// Co se po kliknuti stane, je vec konkretni sceny (vychozi implementace nedelaji nic).
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

    // kazdy snimek: drzene klavesy
    virtual void update(GLFWwindow* window, float deltaTime);

    // vola Application po draw() a pred swapem (cteni stencilu / hloubky)
    void processPendingClick(GLFWwindow* window);

protected:
    virtual void onHudClick(DrawableObject* object);
    virtual void onGroundClick(DrawableObject* ground, const glm::vec3& position);
    virtual void onObjectClick(DrawableObject* object, const glm::vec3& position);
    virtual void onBackgroundClick();

    Scene& scene;

private:
    bool clickPending = false;
    double clickX = 0.0;
    double clickY = 0.0;
};
