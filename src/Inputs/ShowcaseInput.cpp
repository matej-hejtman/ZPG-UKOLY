// Author: Matěj Hejtman, login: HEJ0094
#include "Inputs/ShowcaseInput.h"
#include "Scenes/ShowcaseScene.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <iostream>

static const float PI = 3.14159265f;
static const float FLIGHT_DURATION = 1.8f;
static const float VERTIGO_DURATION = 5.0f;
static const float VERTIGO_CLOSEST = 0.4f;
static const float ORBIT_SPEED_DEG = 14.0f;
static const float ORBIT_RADIUS = 13.0f;
static const float ORBIT_HEIGHT = 6.0f;
static const glm::vec3 ORBIT_LOOK_AT = glm::vec3(0.0f, 2.0f, 0.0f);
static const float MIN_TIME_SPEED = 0.125f;
static const float MAX_TIME_SPEED = 4.0f;
static const float WORLD_SPIN_SPEED = 70.0f;
static const float WORLD_LIFT_SPEED = 2.5f;

ShowcaseInput::ShowcaseInput(ShowcaseScene& scene) : SceneInput(scene), showcase(scene) {}

void ShowcaseInput::printHelp() {
    std::cout << "\n=== SHOWCASE ===\n"
              << "  Kamera   1-6 pohledy,  C prelet kolem sceny,  V dolly zoom (Vertigo),  F baterka\n"
              << "  Svet     N noc/vesmir/mlha,  M miniatura,  sipky toceni/zdvih sveta,  R vratit svet\n"
              << "  Mys      LMB podlaha = shodit kostku,  LMB slunce = supernova,  LMB brana = rozhoupat\n"
              << "           LMB kostka v prstenci = roztocit + reflektor,  LMB HUD = svetla / prostredi\n"
              << "  Hra      K rozhazet zlate kostky ke sbirani (pocitadlo vpravo nahore)\n"
              << "  Cas      Space pauza,  +/- rychlost,  P kyvadla (srovnat/spirala/nuzky),  G prehrat nastup\n"
              << "  Ostatni  L chase svetla,  H HUD,  Backspace uklid,  O dalsi objekt,  I info o objektu,  F1 napoveda\n"
              << std::endl;
}

void ShowcaseInput::onKey(GLFWwindow* window, int key, int action, int mods) {
    SceneInput::onKey(window, key, action, mods);

    if (action != GLFW_PRESS) {
        return;
    }

    static const Shot SHOTS[] = {
        { "prehled",    glm::vec3(0.0f, 4.0f, 15.5f),  glm::vec3(0.0f, 2.0f, 0.0f) },
        { "slunce",     glm::vec3(2.6f, 4.6f, 4.8f),   ShowcaseScene::SUN_POSITION },
        { "materialy",  glm::vec3(1.8f, 2.3f, 8.6f),   glm::vec3(0.0f, 1.75f, 5.0f) },
        { "kyvadla",    glm::vec3(0.0f, 3.6f, -2.6f),  glm::vec3(0.0f, 3.0f, -8.5f) },
        { "DNA",        glm::vec3(-4.4f, 3.6f, 2.8f),  glm::vec3(-8.0f, 3.2f, -1.0f) },
        { "equalizer",  glm::vec3(4.6f, 4.3f, 2.8f),   glm::vec3(8.0f, 0.8f, -1.0f) },
    };

    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        flyTo(SHOTS[key - GLFW_KEY_1]);
        return;
    }

    switch (key) {
    case GLFW_KEY_C:
        if (autopilot == Autopilot::Orbit) {
            stopAutopilot();
            std::cout << "Prelet: vypnut" << std::endl;
        } else {
            startOrbit();
            std::cout << "Prelet: zapnut" << std::endl;
        }
        break;

    case GLFW_KEY_V:
        startVertigo();
        break;

    case GLFW_KEY_N:
        std::cout << "Prostredi: " << showcase.cycleEnvironment() << std::endl;
        break;

    case GLFW_KEY_M:
        std::cout << "Miniatura: " << (showcase.toggleMiniature() ? "zapnuta" : "vypnuta") << std::endl;
        break;

    case GLFW_KEY_R:
        showcase.resetWorld();
        std::cout << "Svet vracen do puvodni polohy" << std::endl;
        break;

    case GLFW_KEY_P:
        std::cout << "Kyvadla: " << showcase.cyclePendulums() << std::endl;
        break;

    case GLFW_KEY_K:
        showcase.startCollectibles();
        std::cout << "Najdi a seber zlate kostky!" << std::endl;
        break;

    case GLFW_KEY_O:
        if (showcase.selectNext()) {
            showcase.printSelection();
        }
        break;

    case GLFW_KEY_I:
        showcase.printSelection();
        break;

    case GLFW_KEY_SPACE:
        togglePause();
        break;

    case GLFW_KEY_EQUAL:
    case GLFW_KEY_KP_ADD:
        changeSpeed(2.0f);
        break;

    case GLFW_KEY_MINUS:
    case GLFW_KEY_KP_SUBTRACT:
        changeSpeed(0.5f);
        break;

    case GLFW_KEY_F:
        std::cout << "Baterka: " << (showcase.toggleFlashlight() ? "zapnuta" : "vypnuta") << std::endl;
        break;

    case GLFW_KEY_L:
        std::cout << "Chase svetla: " << (showcase.toggleChase() ? "zapnuta" : "vypnuta") << std::endl;
        break;

    case GLFW_KEY_G:
        showcase.replayIntro();
        std::cout << "Nastup sceny prehran znovu" << std::endl;
        break;

    case GLFW_KEY_H:
        std::cout << "HUD: " << (showcase.toggleHud() ? "zobrazen" : "skryt") << std::endl;
        break;

    case GLFW_KEY_BACKSPACE:
        showcase.clearSpawned();
        break;

    case GLFW_KEY_F1:
        printHelp();
        break;

    default:
        break;
    }
}

void ShowcaseInput::onMouseMove(GLFWwindow* window, double dx, double dy) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && (dx != 0.0 || dy != 0.0)) {
        stopAutopilot();
    }

    SceneInput::onMouseMove(window, dx, dy);
}

void ShowcaseInput::update(GLFWwindow* window, float deltaTime) {
    if (!helpShown) {
        printHelp();
        helpShown = true;
    }

    auto pressed = [window](int key) {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    for (int key : { GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_Q, GLFW_KEY_E }) {
        if (pressed(key)) {
            stopAutopilot();
            break;
        }
    }

    const float spin = (pressed(GLFW_KEY_RIGHT) ? 1.0f : 0.0f) - (pressed(GLFW_KEY_LEFT) ? 1.0f : 0.0f);
    const float lift = (pressed(GLFW_KEY_UP) ? 1.0f : 0.0f) - (pressed(GLFW_KEY_DOWN) ? 1.0f : 0.0f);
    if (spin != 0.0f || lift != 0.0f) {
        showcase.nudgeWorld(spin * WORLD_SPIN_SPEED * deltaTime, lift * WORLD_LIFT_SPEED * deltaTime);
    }

    SceneInput::update(window, deltaTime);

    if (autopilot == Autopilot::Flight) {
        flightProgress = std::min(flightProgress + deltaTime / FLIGHT_DURATION, 1.0f);
        const float t = flightProgress * flightProgress * (3.0f - 2.0f * flightProgress);

        const glm::vec3 eye = glm::mix(flightFromEye, flightToEye, t);
        const glm::vec3 look = glm::mix(flightFromLook, flightToLook, t);
        const float arc = 2.0f * std::sin(t * PI) * std::min(glm::length(flightToEye - flightFromEye) / 10.0f, 1.0f);

        aim(eye + glm::vec3(0.0f, arc, 0.0f), look);

        if (flightProgress >= 1.0f) {
            autopilot = Autopilot::None;
        }
    } else if (autopilot == Autopilot::Orbit) {
        const float blend = 1.0f - std::exp(-1.5f * deltaTime);
        orbitRadius += (ORBIT_RADIUS - orbitRadius) * blend;
        orbitHeight += (ORBIT_HEIGHT - orbitHeight) * blend;
        orbitAngle += glm::radians(ORBIT_SPEED_DEG) * deltaTime;

        const float bob = 1.2f * std::sin(1.2f * orbitAngle);
        const glm::vec3 eye(orbitRadius * std::cos(orbitAngle), orbitHeight + bob, orbitRadius * std::sin(orbitAngle));
        aim(eye, ORBIT_LOOK_AT);
    } else if (autopilot == Autopilot::Vertigo) {
        vertigoProgress = std::min(vertigoProgress + deltaTime / VERTIGO_DURATION, 1.0f);

        const float s = 0.5f - 0.5f * std::cos(2.0f * PI * vertigoProgress);
        const float distance = vertigoDistance * (1.0f - (1.0f - VERTIGO_CLOSEST) * s);
        const float fov = glm::degrees(2.0f * std::atan(vertigoWidth / (2.0f * distance)));

        aim(vertigoTarget + vertigoDirection * distance, vertigoTarget);
        scene.getCamera()->setFov(fov);

        if (vertigoProgress >= 1.0f) {
            stopAutopilot();
        }
    }
}

void ShowcaseInput::onHudClick(DrawableObject* object) {
    showcase.hudClick(object);
}

void ShowcaseInput::onGroundClick(DrawableObject* ground, const glm::vec3& position) {
    (void)ground;
    showcase.spawnCube(position);
}

void ShowcaseInput::onObjectClick(DrawableObject* object, const glm::vec3& position) {
    (void)position;
    showcase.interact(object);
}

void ShowcaseInput::flyTo(const Shot& shot) {
    stopAutopilot();
    Camera* camera = scene.getCamera();

    flightFromEye = camera->getEye();
    flightFromLook = camera->getEye() + camera->getTarget() * glm::length(shot.lookAt - shot.eye);
    flightToEye = shot.eye;
    flightToLook = shot.lookAt;
    flightProgress = 0.0f;
    autopilot = Autopilot::Flight;

    std::cout << "Kamera: " << shot.name << std::endl;
}

void ShowcaseInput::startOrbit() {
    stopAutopilot();
    const glm::vec3 eye = scene.getCamera()->getEye();

    orbitAngle = std::atan2(eye.z, eye.x);
    orbitRadius = std::max(glm::length(glm::vec2(eye.x, eye.z)), 2.0f);
    orbitHeight = eye.y;
    autopilot = Autopilot::Orbit;
}

void ShowcaseInput::startVertigo() {
    stopAutopilot();
    Camera* camera = scene.getCamera();

    vertigoTarget = showcase.toWorld(ShowcaseScene::SUN_POSITION);
    const glm::vec3 offset = camera->getEye() - vertigoTarget;
    vertigoDistance = glm::length(offset);

    if (vertigoDistance < 1.0f) {
        return;
    }

    vertigoDirection = offset / vertigoDistance;
    vertigoFov = camera->getFov();
    vertigoWidth = 2.0f * vertigoDistance * std::tan(glm::radians(vertigoFov) * 0.5f);
    vertigoProgress = 0.0f;
    autopilot = Autopilot::Vertigo;

    std::cout << "Dolly zoom: slunce zustava stejne velke, svet kolem se natahuje" << std::endl;
}

void ShowcaseInput::stopAutopilot() {
    if (autopilot == Autopilot::Vertigo) {
        scene.getCamera()->setFov(vertigoFov);
    }

    autopilot = Autopilot::None;
}

void ShowcaseInput::aim(const glm::vec3& eye, const glm::vec3& lookAt) {
    const glm::vec3 offset = lookAt - eye;
    if (glm::length(offset) < 1e-4f) {
        return;
    }

    const glm::vec3 direction = glm::normalize(offset);
    const float alpha = glm::degrees(std::acos(glm::clamp(direction.y, -1.0f, 1.0f)));
    const float phi = glm::degrees(std::atan2(direction.z, direction.x));

    scene.getCamera()->setPosition(eye, alpha, phi);
}

void ShowcaseInput::changeSpeed(float factor) {
    SceneClock& clock = scene.getClock();
    clock.setSpeed(std::clamp(clock.getSpeed() * factor, MIN_TIME_SPEED, MAX_TIME_SPEED));
    std::cout << "Rychlost casu: " << clock.getSpeed() << "x" << std::endl;
}

void ShowcaseInput::togglePause() {
    SceneClock& clock = scene.getClock();
    clock.setPaused(!clock.isPaused());
    std::cout << (clock.isPaused() ? "Pauza" : "Pokracuje") << std::endl;
}
