// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/HudPlacement.h"

#include <glm/gtc/matrix_transform.hpp>

HudPlacement::HudPlacement(const float* aspect, HudAnchor anchor, const glm::vec2& offset, float scale) : aspect(aspect), anchor(anchor), offset(offset), scale(scale) {}

glm::mat4 HudPlacement::getMatrix() const {
    glm::vec2 corner(0.0f);

    switch (anchor) {
    case HudAnchor::TopLeft:
        corner = glm::vec2(-*aspect, 1.0f);
        break;

    case HudAnchor::TopRight:
        corner = glm::vec2(*aspect, 1.0f);
        break;

    case HudAnchor::BottomLeft:
        corner = glm::vec2(-*aspect, -1.0f);
        break;

    case HudAnchor::BottomRight:
        corner = glm::vec2(*aspect, -1.0f);
        break;

    default:
        break;
    }

    const glm::vec2 position = corner + offset;
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    
    return glm::scale(matrix, glm::vec3(scale));
}
