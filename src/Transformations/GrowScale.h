// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

class GrowScale : public Transformation {
public:
    GrowScale(const SceneClock& clock, float target, float durationSeconds);
    GrowScale(const SceneClock& clock, const glm::vec3& from, const glm::vec3& to, float durationSeconds, const glm::vec3& pivot = glm::vec3(0.0f));

    glm::mat4 getMatrix() const override;

    void restart();
    bool isFinished() const;
    glm::vec3 getCurrentScale() const;

    void setFrom(const glm::vec3& value);
    const glm::vec3& getFrom() const;

    void setTo(const glm::vec3& value);
    const glm::vec3& getTo() const;

    void setDuration(float seconds);
    float getDuration() const;

    void setPivot(const glm::vec3& value);
    const glm::vec3& getPivot() const;

private:
    float getProgress() const;

    const SceneClock& clock;
    glm::vec3 from;
    glm::vec3 to;
    float duration;
    glm::vec3 pivot;
    double startTime;
};
