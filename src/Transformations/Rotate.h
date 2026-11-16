// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class Rotate : public Transformation {
public:
    Rotate(float angleDeg, const glm::vec3& axis);

    glm::mat4 getMatrix() const override;

    void setAngle(float valueDeg);
    float getAngle() const;

private:
    float angleDeg;
    glm::vec3 axis;
};
