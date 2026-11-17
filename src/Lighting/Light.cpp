// Author: Matěj Hejtman, login: HEJ0094
#include "Lighting/Light.h"

#include <algorithm>
#include <cmath>
#include <iostream>

static const float MAX_SPOT_ANGLE = 89.0f;

void Light::setPosition(const glm::vec3& value) {
    position = value;
}

const glm::vec3& Light::getPosition() const {
    return position;
}

void Light::setColor(const glm::vec3& value) {
    color = value;
}

const glm::vec3& Light::getColor() const {
    return color;
}

void Light::setAttenuation(float constantValue, float linearValue, float quadraticValue) {
    constant = constantValue;
    linear = linearValue;
    quadratic = quadraticValue;
}

float Light::getConstant() const {
    return constant;
}

float Light::getLinear() const {
    return linear;
}

float Light::getQuadratic() const {
    return quadratic;
}

void Light::setEnabled(bool value) {
    enabled = value;
}

bool Light::isEnabled() const {
    return enabled;
}

void Light::setType(LightType value) {
    type = value;
}

LightType Light::getType() const {
    return type;
}

void Light::setDirection(const glm::vec3& value) {
    if (glm::length(value) < 1e-6f) {
        std::cerr << "WARNING: light direction is zero, ignored" << std::endl;
        return;
    }
    direction = glm::normalize(value);
}

const glm::vec3& Light::getDirection() const {
    return direction;
}

void Light::setSpotAngles(float innerDeg, float outerDeg) {
    innerDeg = std::clamp(innerDeg, 0.0f, MAX_SPOT_ANGLE);
    outerDeg = std::clamp(outerDeg, 0.0f, MAX_SPOT_ANGLE);

    if (outerDeg < innerDeg) {
        std::cerr << "WARNING: spot outer angle (" << outerDeg << ") < inner angle (" << innerDeg << "), outer set to inner" << std::endl;
        outerDeg = innerDeg;
    }

    cutOffDeg = innerDeg;
    outerCutOffDeg = outerDeg;
}

void Light::setCutOff(float innerDeg) {
    setSpotAngles(innerDeg, std::max(outerCutOffDeg, innerDeg));
}

float Light::getCutOff() const {
    return cutOffDeg;
}

void Light::setOuterCutOff(float outerDeg) {
    setSpotAngles(std::min(cutOffDeg, outerDeg), outerDeg);
}

float Light::getOuterCutOff() const {
    return outerCutOffDeg;
}

TransformationComposite& Light::getTransformation() {
    return transformation;
}

LightData Light::toData() const {
    const glm::mat4 matrix = transformation.getMatrix();

    const glm::vec4 worldPosition = matrix * glm::vec4(position, 1.0f);
    glm::vec3 worldDirection = glm::vec3(matrix * glm::vec4(direction, 0.0f));
    worldDirection = glm::length(worldDirection) > 1e-6f ? glm::normalize(worldDirection) : direction;

    LightData data;
    data.position = glm::vec4(glm::vec3(worldPosition) / worldPosition.w, 1.0f);
    data.color = glm::vec4(color, 1.0f);
    data.attenuation = glm::vec4(constant, linear, quadratic, 0.0f);
    data.direction = glm::vec4(worldDirection, 0.0f);
    data.params = glm::vec4(static_cast<float>(type),
                            std::cos(glm::radians(cutOffDeg)),
                            std::cos(glm::radians(outerCutOffDeg)),
                            0.0f);
    return data;
}
