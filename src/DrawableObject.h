// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Material.h"

#include <glm/glm.hpp>

class Model;
class ShaderProgram;
class Transformation;
class Texture;

class DrawableObject {
public:
    DrawableObject(Model* model, ShaderProgram* program, Transformation* transformation);

    void setColor(const glm::vec3& value) { color = value; }
    void setProgram(ShaderProgram* value) { program = value; }
    void setMaterial(const Material& value) { material = value; }
    void setTexture(Texture* value) { texture = value; }

    void draw() const;

private:
    Model* model;
    ShaderProgram* program;
    Transformation* transformation;
    glm::vec3 color = glm::vec3(1.0f);
    Material material;                
    Texture* texture = nullptr;       
};
