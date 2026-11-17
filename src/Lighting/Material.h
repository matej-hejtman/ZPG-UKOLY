// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class ShaderProgram;
class Texture;

struct Material {
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(0.5f);
    float shininess = 32.0f;
    Texture* texture = nullptr;

    void apply(ShaderProgram* program) const;

    static Material gold();
    static Material silver();
    static Material copper();
    static Material bronze();
    static Material chrome();
    static Material pearl();
    static Material emerald();
    static Material ruby();
    static Material plastic();
    static Material rubber();
};
