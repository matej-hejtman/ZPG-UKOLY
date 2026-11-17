// Author: Matěj Hejtman, login: HEJ0094
#include "Core/SceneClock.h"

#include <algorithm>

void SceneClock::advance(float realDeltaTime) {
    delta = paused ? 0.0f : realDeltaTime * speed;
    time += delta;
}

double SceneClock::getTime() const {
    return time;
}

float SceneClock::getDelta() const {
    return delta;
}

void SceneClock::setPaused(bool value) {
    paused = value;
}

bool SceneClock::isPaused() const {
    return paused;
}

void SceneClock::setSpeed(float value) {
    speed = std::max(value, 0.0f);
}

float SceneClock::getSpeed() const {
    return speed;
}
