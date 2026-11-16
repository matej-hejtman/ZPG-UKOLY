// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class LineMove : public Transformation {
public:
    LineMove(const glm::vec3& a, const glm::vec3& b, float unitsPerSecond, bool faceDirection = false);

    glm::mat4 getMatrix() const override;

    float getT() const;
    bool isReturning() const;
    glm::vec3 getPosition() const;

private:
    glm::vec3 a;
    glm::vec3 b;
    float unitsPerSecond;
    bool faceDirection;
    double startTime;
};
