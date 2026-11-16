// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/GrowScale.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

GrowScale::GrowScale(float target, float durationSeconds) : target(target), duration(durationSeconds), startTime(glfwGetTime()) {}

glm::mat4 GrowScale::getMatrix() const {
    float x = duration > 0.0f ? static_cast<float>((glfwGetTime() - startTime) / duration) : 1.0f;
    x = glm::clamp(x, 0.0f, 1.0f);
    const float eased = x * x * (3.0f - 2.0f * x);
    return glm::scale(glm::mat4(1.0f), glm::vec3(target * eased));
}
