// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/Oscillate.h"
#include "Core/SceneClock.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

Oscillate::Oscillate(const SceneClock& clock, float amplitudeDeg, float frequencyHz, const glm::vec3& axis, float phaseDeg) : clock(clock), amplitudeDeg(amplitudeDeg), frequencyHz(frequencyHz), axis(axis), phaseDeg(phaseDeg), startTime(clock.getTime()) {}

glm::mat4 Oscillate::getMatrix() const {
    const double elapsed = clock.getTime() - startTime;
    const double argument = 2.0 * glm::pi<double>() * frequencyHz * elapsed + glm::radians(static_cast<double>(phaseDeg));
    const float angle = amplitudeDeg * static_cast<float>(std::sin(argument));
    
    return glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
}

void Oscillate::setPhase(float value) {
    phaseDeg = value;
}

float Oscillate::getPhase() const {
    return phaseDeg;
}
