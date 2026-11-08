// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class Light {
public:
    void setPosition(const glm::vec3& value) { position = value; }
    void setColor(const glm::vec3& value) { color = value; }
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getColor() const { return color; }

    void setAttenuation(float constantValue, float linearValue, float quadraticValue) {
        constant = constantValue;
        linear = linearValue;
        quadratic = quadraticValue;
    }

    float getConstant() const { return constant; }
    float getLinear() const { return linear; }
    float getQuadratic() const { return quadratic; }

private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};
