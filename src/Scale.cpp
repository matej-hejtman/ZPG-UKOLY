// Author: Matt Hejtman, login: HEJ0094
#include "Scale.h"
#include <glm/gtc/matrix_transform.hpp>

Scale::Scale(const glm::vec3& factor) : factor(factor) {}

Scale::Scale(float factor) : factor(factor, factor, factor) {}

glm::mat4 Scale::getMatrix() const {
    return glm::scale(glm::mat4(1.0f), factor);
}
