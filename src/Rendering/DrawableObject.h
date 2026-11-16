// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Lighting/Material.h"

#include <glm/glm.hpp>

class Model;
class ShaderProgram;
class Transformation;
class Texture;

class DrawableObject {
public:
    enum class Kind { Normal, Ground, Collectible, Moving };

    DrawableObject(Model* model, ShaderProgram* program, Transformation* transformation);

    void setColor(const glm::vec3& value);
    const glm::vec3& getColor() const;

    void setProgram(ShaderProgram* value);
    void setMaterial(const Material& value);
    void setTexture(Texture* value);

    void setId(unsigned value);
    unsigned getId() const;

    void setKind(Kind value);
    Kind getKind() const;

    static const char* kindName(Kind kind);

    void draw() const;

private:
    Model* model;
    ShaderProgram* program;
    Transformation* transformation;
    glm::vec3 color = glm::vec3(1.0f);
    Material material;
    Texture* texture = nullptr;
    unsigned id = 0;
    Kind kind = Kind::Normal;
};
