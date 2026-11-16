// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class GrowScale : public Transformation {
public:
    GrowScale(float target, float durationSeconds);

    glm::mat4 getMatrix() const override;

private:
    float target;
    float duration;
    double startTime;
};
