// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Transformation.h"

class Translate : public Transformation {
public:
    explicit Translate(const glm::vec3& offset);

    glm::mat4 getMatrix() const override;

    void setOffset(const glm::vec3& value) { offset = value; }
    const glm::vec3& getOffset() const { return offset; }

private:
    glm::vec3 offset;
};
