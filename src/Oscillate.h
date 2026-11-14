// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class Oscillate : public Transformation {
public:
    Oscillate(float amplitudeDeg, float frequencyHz, const glm::vec3& axis);

    glm::mat4 getMatrix() const override;

private:
    float amplitudeDeg;
    float frequencyHz;
    glm::vec3 axis;
    double startTime;
};
