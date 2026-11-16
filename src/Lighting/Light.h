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

	void setEnabled(bool value) { enabled = value; }
	bool isEnabled() const { return enabled; }

    void setType(LightType value) { type = value; }
	LightType getType() const { return type; }

    void setDirection(const glm::vec3& value) { direction = value; }
    const glm::vec3& getDirection() const { return direction; }

    void setCutOff(float value) { cutOff = value; }
    float getCutOff() const { return cutOff; }

    void setOuterCutOff(float value) { outerCutOff = value; }
	float getOuterCutOff() const { return outerCutOff; }

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
