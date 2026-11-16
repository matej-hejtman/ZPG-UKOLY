// Author: Matěj Hejtman, login: HEJ0094
#include "Oscillate.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

Oscillate::Oscillate(float amplitudeDeg, float frequencyHz, const glm::vec3& axis) : amplitudeDeg(amplitudeDeg), frequencyHz(frequencyHz), axis(axis), startTime(glfwGetTime()) {}

glm::mat4 Oscillate::getMatrix() const {
    const double elapsed = glfwGetTime() - startTime;
    const float angle = amplitudeDeg * static_cast<float>(std::sin(2.0 * 3.14159265358979 * frequencyHz * elapsed));
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}
