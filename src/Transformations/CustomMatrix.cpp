// Author: Matěj Hejtman, login: HEJ0094
#include "Transformations/CustomMatrix.h"

CustomMatrix::CustomMatrix(const glm::mat4& matrix) : matrix(matrix) {}

glm::mat4 CustomMatrix::getMatrix() const {
    return matrix;
}
