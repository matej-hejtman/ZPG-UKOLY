// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

class Oscillate : public Transformation {
public:
    Oscillate(const SceneClock& clock, float amplitudeDeg, float frequencyHz, const glm::vec3& axis, float phaseDeg = 0.0f);

    glm::mat4 getMatrix() const override;

    void setPhase(float phaseDeg);
    float getPhase() const;

private:
    const SceneClock& clock;
    float amplitudeDeg;
    float frequencyHz;
    glm::vec3 axis;
    float phaseDeg;
    double startTime;
};
