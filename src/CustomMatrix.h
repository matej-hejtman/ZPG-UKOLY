// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class CustomMatrix : public Transformation {
public:
    explicit CustomMatrix(const glm::mat4& matrix) : matrix(matrix) {}
    glm::mat4 getMatrix() const override { return matrix; }

private:
    glm::mat4 matrix;
};
