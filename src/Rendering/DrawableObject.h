// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Material.h"

#include <glm/glm.hpp>

class Model;
class ShaderProgram;
class Transformation;
class Texture;

class DrawableObject {
public:
    enum class Kind { Normal, Ground, Collectible, Moving };

    DrawableObject(Model* model, ShaderProgram* program, Transformation* transformation);

    void setColor(const glm::vec3& value) { 
        color = value; 
    }
    
    const glm::vec3& getColor() const { 
        return color; 
    }
    
    void setProgram(ShaderProgram* value) { 
        program = value; 
    }

    void setMaterial(const Material& value) { 
        material = value; 
    }

    void setTexture(Texture* value) { 
        texture = value; 
    }

    void setId(unsigned value) { 
        id = value; 
    }

    unsigned getId() const { 
        return id; 
    }

    void setKind(Kind value) { 
        kind = value; 
    }
    
    Kind getKind() const { 
        return kind; 
    }

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
