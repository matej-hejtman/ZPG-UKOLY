// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class DynamicRotate : public Transformation {
public:
    DynamicRotate(float degPerSecond, const glm::vec3& axis);

    glm::mat4 getMatrix() const override;

    float getAngle() const;
    void setSpeed(float degPerSecond);
    float getSpeed() const { return degPerSecond; }

private:
    float degPerSecond;
    glm::vec3 axis;
    float startAngle = 0.0f;
    double startTime;
};
