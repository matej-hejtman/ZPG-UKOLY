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

    Model* getModel() const;

    // POZOR: novy shader musi byt pripojeny ke kamere sceny -> scene.registerProgram(shader)
    void setProgram(ShaderProgram* value);
    ShaderProgram* getProgram() const;

    void setTransformation(Transformation* value);
    Transformation* getTransformation() const;

    // POZOR: setMaterial prepise i texturu (je soucasti materialu)
    void setMaterial(const Material& value);
    const Material& getMaterial() const;

    // zkratka pro material.texture, nullptr = bez textury
    void setTexture(Texture* value);
    Texture* getTexture() const;

    // skryty objekt se nekresli a neda se na nej kliknout, ale zustava ve scene i s ID
    void setVisible(bool value);
    bool isVisible() const;

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
    bool visible = true;
    unsigned id = 0;
    Kind kind = Kind::Normal;
};
