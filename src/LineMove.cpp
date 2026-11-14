// Author: Matt Hejtman, login: HEJ0094
#include "LineMove.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

LineMove::LineMove(const glm::vec3& a, const glm::vec3& b, float unitsPerSecond) : a(a), b(b), unitsPerSecond(unitsPerSecond), startTime(glfwGetTime()) {}

float LineMove::getT() const {
    const float length = glm::length(b - a);
    if (length < 1e-4f) {
        return 0.0f;
    }

    const double s = unitsPerSecond * (glfwGetTime() - startTime) / length;

    return static_cast<float>(1.0 - std::fabs(1.0 - std::fmod(s, 2.0)));
}

glm::vec3 LineMove::getPosition() const {
    const float t = getT();
    return (1.0f - t) * a + t * b;
}

glm::mat4 LineMove::getMatrix() const {
    return glm::translate(glm::mat4(1.0f), getPosition());
}
