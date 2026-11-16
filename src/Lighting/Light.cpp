// Author: Matěj Hejtman, login: HEJ0094
#include "Lighting/Light.h"

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
    direction = value;
}

const glm::vec3& Light::getDirection() const {
    return direction;
}

void Light::setCutOff(float value) {
    cutOff = value;
}

float Light::getCutOff() const {
    return cutOff;
}

void Light::setOuterCutOff(float value) {
    outerCutOff = value;
}

float Light::getOuterCutOff() const {
    return outerCutOff;
}
