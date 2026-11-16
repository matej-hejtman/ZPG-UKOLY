// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class CustomMatrix : public Transformation {
public:
    explicit CustomMatrix(const glm::mat4& matrix);

    glm::mat4 getMatrix() const override;

private:
    glm::mat4 matrix;
};
