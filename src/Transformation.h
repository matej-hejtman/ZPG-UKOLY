// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class Transformation {
public:
    virtual ~Transformation() = default;
    virtual glm::mat4 getMatrix() const = 0;
};
