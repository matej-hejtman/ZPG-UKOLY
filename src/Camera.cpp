// Author: Matt Hejtman, login: HEJ0094
#include "Camera.h"
#include "ICameraObserver.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

Camera::Camera() {
    updateTarget();
}

void Camera::attach(ICameraObserver* observer) {
    observers.push_back(observer);
}

void Camera::notifyAll() {
    for (ICameraObserver* observer : observers) {
        observer->onCameraChanged(this);
    }
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(eye, eye + target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::setPosition(const glm::vec3& eyePosition, float alphaDeg, float phiDeg) {
    eye = eyePosition;
    alpha = std::clamp(alphaDeg, 1.0f, 179.0f);
    phi = phiDeg;
    updateTarget();
    notifyAll();
}

void Camera::updateTarget() {
    const float a = glm::radians(alpha);
    const float p = glm::radians(phi);
    target = glm::vec3(std::sin(a) * std::cos(p), std::cos(a), std::sin(a) * std::sin(p));
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(target, up));
}

void Camera::rotate(float dx, float dy)   {
    phi += dx * sensitivity;
    alpha += dy * sensitivity;
    alpha = std::clamp(alpha, 1.0f, 179.0f);
    updateTarget();
    notifyAll();
}

void Camera::moveForward(float deltaTime) {
    eye += target * speed * deltaTime;
    notifyAll();
}

void Camera::moveBackward(float deltaTime) {
    eye -= target * speed * deltaTime;
    notifyAll();
}

void Camera::moveLeft(float deltaTime) {
    eye -= right() * speed * deltaTime;
    notifyAll();
}

void Camera::moveRight(float deltaTime) {
    eye += right() * speed * deltaTime;
    notifyAll();
}

void Camera::moveUp(float deltaTime) {
    eye += up * speed * deltaTime;
    notifyAll();
}

void Camera::moveDown(float deltaTime) {
    eye -= up * speed * deltaTime;
    notifyAll();
}

void Camera::setFov(float degrees) {
    fov = std::clamp(degrees, 1.0f, 179.0f);
    notifyAll();
}

void Camera::setAspect(int width, int height) {
    if (width <= 0 || height <= 0){
        return; 
    }
    
    aspect = static_cast<float>(width) / static_cast<float>(height);
    notifyAll();
}
