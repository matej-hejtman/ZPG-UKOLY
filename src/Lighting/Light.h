// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

enum class LightType {
    Point = 0,
    Directional = 1,
    Spot = 2
};

class Light {
public:
    void setPosition(const glm::vec3& value);
    const glm::vec3& getPosition() const;

    void setColor(const glm::vec3& value);
    const glm::vec3& getColor() const;

    void setAttenuation(float constantValue, float linearValue, float quadraticValue);
    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;

    void setEnabled(bool value);
    bool isEnabled() const;

    void setType(LightType value);
    LightType getType() const;

    void setDirection(const glm::vec3& value);
    const glm::vec3& getDirection() const;

    void setCutOff(float value);
    float getCutOff() const;

    void setOuterCutOff(float value);
    float getOuterCutOff() const;

private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    bool enabled = true;

    LightType type = LightType::Point;
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float cutOff = 0.99f;
	float outerCutOff = 0.97f;
};
