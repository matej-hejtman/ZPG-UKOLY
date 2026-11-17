// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

class DynamicRotate : public Transformation {
public:
    DynamicRotate(const SceneClock& clock, float degPerSecond, const glm::vec3& axis, float startAngleDeg = 0.0f);

    glm::mat4 getMatrix() const override;

    float getAngle() const;

    void setSpeed(float degPerSecond);
    float getSpeed() const;

private:
    const SceneClock& clock;
    float degPerSecond;
    glm::vec3 axis;
    float startAngle;
    double startTime;
};
