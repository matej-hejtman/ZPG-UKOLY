// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Inputs/MainInput.h"
#include "Inputs/SceneInput.h"
#include "Core/Resources.h"
#include "Core/Scene.h"

#include <cstddef>
#include <memory>
#include <vector>

struct GLFWwindow;

// Okno, hlavni smycka a prepinani scen.
// O obsahu scen nic nevi: kazda scena ma svuj input a Application
// jen predava GLFW udalosti (nejdriv MainInput, pak input aktivni sceny).
class Application {
public:
    Application();
    ~Application();

    void initialization();
    void createScenes();
    void run();

    // pro MainInput
    void close();
    void switchScene(int step);
    void toggleFullscreen();

private:
    struct SceneEntry {
        std::unique_ptr<Scene> scene;
        std::unique_ptr<SceneInput> input;
    };

    // scena + jeji input; InputT dostane konkretni typ sceny (SceneT&)
    template <class SceneT, class InputT>
    void addScene() {
        auto scene = std::make_unique<SceneT>(*resources);
        scene->build();     // vychozi zaklad + create()
        auto input = std::make_unique<InputT>(*scene);
        scenes.push_back({ std::move(scene), std::move(input) });
    }

    void activateScene(std::size_t index);
    SceneEntry* active();

    static Application* from(GLFWwindow* window);
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double x, double y);
    static void scrollCallback(GLFWwindow* window, double dx, double dy);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window = nullptr;

    std::unique_ptr<Resources> resources;
    std::vector<SceneEntry> scenes;
    std::size_t activeScene = 0;
    MainInput mainInput{ *this };

    double lastCursorX = 0.0;
    double lastCursorY = 0.0;

    bool fullscreen = false;
    int windowedX = 100, windowedY = 100, windowedWidth = 800, windowedHeight = 600;
};
