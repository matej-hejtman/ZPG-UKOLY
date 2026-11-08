// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class Rotate : public Transformation {
public:
    Rotate(float angleDeg, const glm::vec3& axis);

    glm::mat4 getMatrix() const override;

    void setAngle(float valueDeg) { angleDeg = valueDeg; }
    float getAngle() const { return angleDeg; }

private:
    float angleDeg;
    glm::vec3 axis;
};
