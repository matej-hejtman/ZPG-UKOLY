// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/GrowScale.h"
#include "Core/SceneClock.h"

#include <glm/gtc/matrix_transform.hpp>

GrowScale::GrowScale(const SceneClock& clock, float target, float durationSeconds)
    : GrowScale(clock, glm::vec3(0.0f), glm::vec3(target), durationSeconds) {}

GrowScale::GrowScale(const SceneClock& clock, const glm::vec3& from, const glm::vec3& to, float durationSeconds, const glm::vec3& pivot)
    : clock(clock), from(from), to(to), duration(durationSeconds), pivot(pivot), startTime(clock.getTime()) {}

float GrowScale::getProgress() const {
    float x = duration > 0.0f ? static_cast<float>((clock.getTime() - startTime) / duration) : 1.0f;
    x = glm::clamp(x, 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

glm::vec3 GrowScale::getCurrentScale() const {
    return glm::mix(from, to, getProgress());
}

glm::mat4 GrowScale::getMatrix() const {
    // meritko kolem pivotu: posun pivot do pocatku, zmen meritko, posun zpet
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), pivot);
    matrix = glm::scale(matrix, getCurrentScale());
    return glm::translate(matrix, -pivot);
}

void GrowScale::restart() {
    startTime = clock.getTime();
}

bool GrowScale::isFinished() const {
    return clock.getTime() - startTime >= duration;
}

void GrowScale::setFrom(const glm::vec3& value) {
    from = value;
}

const glm::vec3& GrowScale::getFrom() const {
    return from;
}

void GrowScale::setTo(const glm::vec3& value) {
    to = value;
}

const glm::vec3& GrowScale::getTo() const {
    return to;
}

void GrowScale::setDuration(float seconds) {
    duration = seconds;
}

float GrowScale::getDuration() const {
    return duration;
}

void GrowScale::setPivot(const glm::vec3& value) {
    pivot = value;
}

const glm::vec3& GrowScale::getPivot() const {
    return pivot;
}
