// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class LineMove : public Transformation {
public:
    LineMove(const glm::vec3& a, const glm::vec3& b, float unitsPerSecond);

    glm::mat4 getMatrix() const override;

    float getT() const;
    glm::vec3 getPosition() const;

private:
    glm::vec3 a;
    glm::vec3 b;
    float unitsPerSecond;
    double startTime;
};
