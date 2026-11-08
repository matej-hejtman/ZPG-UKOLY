// Author: Matt Hejtman, login: HEJ0094
#include "Rotate.h"
#include <glm/gtc/matrix_transform.hpp>

Rotate::Rotate(float angleDeg, const glm::vec3& axis) : angleDeg(angleDeg), axis(axis) {}

glm::mat4 Rotate::getMatrix() const {
    return glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), axis);
}
