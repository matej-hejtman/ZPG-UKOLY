// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Transformations/Transformation.h"

class Translate : public Transformation {
public:
    explicit Translate(const glm::vec3& offset);

    glm::mat4 getMatrix() const override;

    void setOffset(const glm::vec3& value);
    const glm::vec3& getOffset() const;

private:
    glm::vec3 offset;
};
