// Author: Matt Hejtman, login: HEJ0094
#include "Translate.h"
#include <glm/gtc/matrix_transform.hpp>

Translate::Translate(const glm::vec3& offset) : offset(offset) {}

glm::mat4 Translate::getMatrix() const {
    return glm::translate(glm::mat4(1.0f), offset);
}
