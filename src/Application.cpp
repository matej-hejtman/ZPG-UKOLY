// Author: Matěj Hejtman, login: HEJ0094
#include "Application.h"

#include "Scenes/TestScene.h"
#include "Inputs/TestSceneInput.h"
#include "Scenes/ShowcaseScene.h"
#include "Inputs/ShowcaseInput.h"
#include "Scenes/CV2Scene.h"
#include "Scenes/CV3Scene.h"
#include "Scenes/CV4Scene.h"
#include "Scenes/CV5Scene.h"
#include "Scenes/CV6Scene.h"
#include "Scenes/CV7Scene.h"
#include "Scenes/CV8Scene.h"
#include "Scenes/CV9Scene.h"
#include "Scenes/CV10Scene.h"

#include "Inputs/SceneInput.h"

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>

//=================== PRIDAVANI SCEN ===================

void Application::createScenes() {

//  addScene<Scene, SceneInput>();

    addScene<ShowcaseScene, ShowcaseInput>();
    addScene<TestScene, TestSceneInput>();
	addScene<CV2Scene, SceneInput>();
    addScene<CV3Scene, SceneInput>();
    addScene<CV4Scene, SceneInput>();
    addScene<CV5Scene, SceneInput>();
    addScene<CV6Scene, SceneInput>();
    addScene<CV7Scene, SceneInput>();
    addScene<CV8Scene, SceneInput>();
    addScene<CV9Scene, SceneInput>();
    addScene<CV10Scene, SceneInput>();

    activateScene(0);
}

//======================================================

Application::Application() = default;

Application::~Application() = default;

void Application::initialization() {
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        std::cerr << "ERROR: could not start GLFW" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);

    window = glfwCreateWindow(windowedWidth, windowedHeight, "ZPG", nullptr, nullptr);

    if (!window) {
        std::cerr << "ERROR: could not create window" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "ERROR: could not load OpenGL functions (GLAD)" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL:           " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GLFW:           " << glfwGetVersionString() << std::endl;

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwGetCursorPos(window, &lastCursorX, &lastCursorY);

    resources = std::make_unique<Resources>();
}

void Application::run() {
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const float deltaTime = static_cast<float>(now - lastTime);
        lastTime = now;

        SceneEntry* entry = active();

        if (entry) {
            entry->input->update(window, deltaTime);
            entry->scene->tick(deltaTime);

            const glm::vec3& background = entry->scene->getClearColor();
            glClearColor(background.r, background.g, background.b, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (entry) {
            entry->scene->draw();
            entry->input->processPendingClick(window);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scenes.clear();
    resources.reset();

    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
}


Application::SceneEntry* Application::active() {
    return activeScene < scenes.size() ? &scenes[activeScene] : nullptr;
}

void Application::activateScene(std::size_t index) {
    if (index >= scenes.size()) {
        return;
    }

    activeScene = index;

    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    scenes[activeScene].scene->activate(width, height);
}

void Application::switchScene(int step) {
    if (scenes.empty()) {
        return;
    }

    const int count = static_cast<int>(scenes.size());
    const int next = ((static_cast<int>(activeScene) + step) % count + count) % count;
    activateScene(static_cast<std::size_t>(next));
    std::cout << "Active scene: " << next + 1 << " / " << count << std::endl;
}

void Application::close() {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Application::toggleFullscreen() {
    if (!fullscreen) {
        glfwGetWindowPos(window, &windowedX, &windowedY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(window, nullptr, windowedX, windowedY, windowedWidth, windowedHeight, 0);
    }

    fullscreen = !fullscreen;
}


Application* Application::from(GLFWwindow* window) {
    return static_cast<Application*>(glfwGetWindowUserPointer(window));
}

void Application::errorCallback(int error, const char* description) {
    std::cerr << "GLFW error " << error << ": " << description << std::endl;
}

void Application::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods) {
    Application* app = from(window);
    if (!app) {
        return;
    }

    if (app->mainInput.onKey(key, action, mods)) {
        return;
    }

    if (SceneEntry* entry = app->active()) {
        entry->input->onKey(window, key, action, mods);
    }
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = from(window);
    if (!app) {
        return;
    }

    if (SceneEntry* entry = app->active()) {
        entry->input->onMouseButton(window, button, action, mods);
    }
}

void Application::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    Application* app = from(window);
    if (!app) {
        return;
    }

    const double dx = x - app->lastCursorX;
    const double dy = y - app->lastCursorY;
    app->lastCursorX = x;
    app->lastCursorY = y;

    if (SceneEntry* entry = app->active()) {
        entry->input->onMouseMove(window, dx, dy);
    }
}

void Application::scrollCallback(GLFWwindow* window, double dx, double dy) {
    Application* app = from(window);
    if (!app) {
        return;
    }

    if (SceneEntry* entry = app->active()) {
        entry->input->onScroll(window, dx, dy);
    }
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Application* app = from(window);
    if (!app) {
        return;
    }

    if (SceneEntry* entry = app->active()) {
        entry->scene->resize(width, height);
    }
}
