// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

// Kotva HUD prvku na obrazovce
enum class HudAnchor {
    Center,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

// Umisteni HUD prvku. HUD souradnice: y od -1 (dole) do 1 (nahore),
// x od -aspect (vlevo) do +aspect (vpravo), takze se prvky pri resize nedeformuji.
// offset je posun od kotvy (napr. TopLeft + (0.2, -0.1) = kousek doprava a dolu).
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
