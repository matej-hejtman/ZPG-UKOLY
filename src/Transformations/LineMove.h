// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

// Pohyb tam a zpet mezi body A a B danou rychlosti.
// faceDirection = natoci objekt kolem Y po smeru pohybu.
// POZOR: natoceni predpoklada, ze model "kouka" po +Z (predek modelu k +Z).
class LineMove : public Transformation {
public:
    LineMove(const SceneClock& clock, const glm::vec3& a, const glm::vec3& b, float unitsPerSecond, bool faceDirection = false);

    glm::mat4 getMatrix() const override;

    float getT() const;             // 0 = v bode A, 1 = v bode B
    bool isReturning() const;       // true = cestou z B do A
    glm::vec3 getPosition() const;

private:
    struct State {
        float t = 0.0f;
        bool returning = false;
    };

    // pozice i smer z JEDNOHO cteni casu, aby se v bode obratu neshodly
    State getState() const;

    const SceneClock& clock;
    glm::vec3 a;
    glm::vec3 b;
    float unitsPerSecond;
    bool faceDirection;
    double startTime;
};
