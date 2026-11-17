// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

enum class HudAnchor {
    Center,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

class HudPlacement : public Transformation {
public:
    HudPlacement(const float* aspect, HudAnchor anchor, const glm::vec2& offset, float scale);

    glm::mat4 getMatrix() const override;

private:
    const float* aspect;
    HudAnchor anchor;
    glm::vec2 offset;
    float scale;
};
