// Author: Matt Hejtman, login: HEJ0094
#include "DynamicRotate.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

DynamicRotate::DynamicRotate(float degPerSecond, const glm::vec3& axis) : degPerSecond(degPerSecond), axis(axis), startTime(glfwGetTime()) {}

float DynamicRotate::getAngle() const {
    const double elapsed = glfwGetTime() - startTime;
    return startAngle + static_cast<float>(degPerSecond * elapsed);
}

void DynamicRotate::setSpeed(float value) {
    startAngle = getAngle();
    startTime = glfwGetTime();
    degPerSecond = value;
}

glm::mat4 DynamicRotate::getMatrix() const {
    return glm::rotate(glm::mat4(1.0f), glm::radians(getAngle()), axis);
}
