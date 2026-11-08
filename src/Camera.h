// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>
#include <vector>

class ICameraObserver;

class Camera
{
public:
    Camera();

    void attach(ICameraObserver* observer);
    void notifyAll();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setPosition(const glm::vec3& eyePosition, float alphaDeg, float phiDeg);
    void setSpeed(float unitsPerSecond) { speed = unitsPerSecond; }

    void rotate(float dx, float dy);
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);
    void setAspect(int width, int height);
    void setFov(float degrees);
    float getFov() const { return fov; }

    const glm::vec3& getEye() const { return eye; }
    const glm::vec3& getTarget() const { return target; }

private:
    void updateTarget();
    glm::vec3 right() const;

    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float alpha = 90.0f;
    float phi = -90.0f;
    float fov = 60.0f;
    float aspect = 4.0f / 3.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    float speed = 1.5f;
    float sensitivity = 0.15f;

    std::vector<ICameraObserver*> observers;
};
