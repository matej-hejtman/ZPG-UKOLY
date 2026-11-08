// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class ShaderProgram;

class Material {
public:
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 32.0f;

    void apply(ShaderProgram* program) const;
};
