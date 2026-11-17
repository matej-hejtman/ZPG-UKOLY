// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

class LineMove : public Transformation {
public:
    LineMove(const SceneClock& clock, const glm::vec3& a, const glm::vec3& b, float unitsPerSecond, bool faceDirection = false);

    glm::mat4 getMatrix() const override;

    float getT() const;
    bool isReturning() const;
    glm::vec3 getPosition() const;

private:
    struct State {
        float t = 0.0f;
        bool returning = false;
    };

    State getState() const;

    const SceneClock& clock;
    glm::vec3 a;
    glm::vec3 b;
    float unitsPerSecond;
    bool faceDirection;
    double startTime;
};
