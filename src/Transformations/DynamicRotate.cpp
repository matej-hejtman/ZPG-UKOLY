// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/DynamicRotate.h"
#include "Core/SceneClock.h"

#include <glm/gtc/matrix_transform.hpp>

DynamicRotate::DynamicRotate(const SceneClock& clock, float degPerSecond, const glm::vec3& axis, float startAngleDeg)
    : clock(clock), degPerSecond(degPerSecond), axis(axis), startAngle(startAngleDeg), startTime(clock.getTime()) {}

float DynamicRotate::getAngle() const {
    const double elapsed = clock.getTime() - startTime;
    return startAngle + static_cast<float>(degPerSecond * elapsed);
}

void DynamicRotate::setSpeed(float value) {
    startAngle = getAngle();
    startTime = clock.getTime();
    degPerSecond = value;
}

float DynamicRotate::getSpeed() const {
    return degPerSecond;
}

glm::mat4 DynamicRotate::getMatrix() const {
    return glm::rotate(glm::mat4(1.0f), glm::radians(getAngle()), axis);
}
