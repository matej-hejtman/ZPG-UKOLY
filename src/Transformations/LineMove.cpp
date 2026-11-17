// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/LineMove.h"
#include "Core/SceneClock.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

LineMove::LineMove(const SceneClock& clock, const glm::vec3& a, const glm::vec3& b, float unitsPerSecond, bool faceDirection) : clock(clock), a(a), b(b), unitsPerSecond(unitsPerSecond), faceDirection(faceDirection), startTime(clock.getTime()) {}

LineMove::State LineMove::getState() const {
    State state;
    const float length = glm::length(b - a);

    if (length < 1e-4f) {
        return state;
    }

    const double s = std::fmod(unitsPerSecond * (clock.getTime() - startTime) / length, 2.0);
    state.returning = s >= 1.0;
    state.t = static_cast<float>(state.returning ? 2.0 - s : s);
    
    return state;
}

float LineMove::getT() const {
    return getState().t;
}

bool LineMove::isReturning() const {
    return getState().returning;
}

glm::vec3 LineMove::getPosition() const {
    const float t = getState().t;
    
    return (1.0f - t) * a + t * b;
}

glm::mat4 LineMove::getMatrix() const {
    const State state = getState();
    const glm::vec3 position = (1.0f - state.t) * a + state.t * b;
    glm::mat4 matrix = glm::translate(glm::mat4(1.0f), position);

    if (faceDirection) {
        const glm::vec3 direction = state.returning ? a - b : b - a;
        if (direction.x != 0.0f || direction.z != 0.0f) {
            matrix = glm::rotate(matrix, std::atan2(direction.x, direction.z), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    return matrix;
}
