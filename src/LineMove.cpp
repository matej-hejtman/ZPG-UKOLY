// Author: Matt Hejtman, login: HEJ0094
#include "LineMove.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

LineMove::LineMove(const glm::vec3& a, const glm::vec3& b, float unitsPerSecond, bool faceDirection) : a(a), b(b), unitsPerSecond(unitsPerSecond), faceDirection(faceDirection), startTime(glfwGetTime()) {}

float LineMove::getT() const {
    const float length = glm::length(b - a);
    if (length < 1e-4f) {
        return 0.0f;
    }

    const double s = unitsPerSecond * (glfwGetTime() - startTime) / length;

    return static_cast<float>(1.0 - std::fabs(1.0 - std::fmod(s, 2.0)));
}

bool LineMove::isReturning() const {
    const float length = glm::length(b - a);
    if (length < 1e-4f) {
        return false;
    }

    const double s = unitsPerSecond * (glfwGetTime() - startTime) / length;
    return std::fmod(s, 2.0) >= 1.0;
}

glm::vec3 LineMove::getPosition() const {
    const float t = getT();
    return (1.0f - t) * a + t * b;
}

glm::mat4 LineMove::getMatrix() const {
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), getPosition());

    if (faceDirection) {
        const glm::vec3 direction = isReturning() ? a - b : b - a;
        if (direction.x != 0.0f || direction.z != 0.0f) {
            matrix = glm::rotate(matrix, std::atan2(direction.x, direction.z), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    return matrix;
}
