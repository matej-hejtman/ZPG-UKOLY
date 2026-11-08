// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

// Leaf: scaling (non-uniform with a vector, uniform with a single number)
class Scale : public Transformation {
public:
    explicit Scale(const glm::vec3& factor);
    explicit Scale(float factor);

    glm::mat4 getMatrix() const override;

    void setFactor(const glm::vec3& value) { factor = value; }
    const glm::vec3& getFactor() const { return factor; }

private:
    glm::vec3 factor;
};
