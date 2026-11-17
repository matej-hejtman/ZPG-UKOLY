// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class SceneClock;

// Plynula zmena meritka z "from" na "to" za danou dobu (smoothstep).
//   - from > to   ... zmensovani (napr. mizeni objektu)
//   - vec3        ... nerovnomerne (napr. roste jen do vysky)
//   - pivot       ... bod, kolem ktereho se meni meritko (v souradnicich modelu):
//                     stred modelu = roste do vsech stran,
//                     spodek modelu = roste nahoru, vrsek = roste dolu
class GrowScale : public Transformation {
public:
    // z nuly na target, kolem pocatku modelu
    GrowScale(const SceneClock& clock, float target, float durationSeconds);
    GrowScale(const SceneClock& clock, const glm::vec3& from, const glm::vec3& to, float durationSeconds, const glm::vec3& pivot = glm::vec3(0.0f));

    glm::mat4 getMatrix() const override;

    void restart();                 // zacne znovu od "from"
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
    float getProgress() const;      // 0..1 (uz s easingem)

    const SceneClock& clock;
    glm::vec3 from;
    glm::vec3 to;
    float duration;
    glm::vec3 pivot;
    double startTime;
};
