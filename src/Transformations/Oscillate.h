// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

// Kyvani (sinus) kolem osy: uhel = amplituda * sin(2*pi*f*t + faze).
// Faze posune kyvadlo v case, takze vic kyvadel nekyva synchronne.
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
