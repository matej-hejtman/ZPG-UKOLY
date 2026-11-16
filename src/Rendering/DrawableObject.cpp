// Author: Matěj Hejtman, login: HEJ0094
#include "Rendering/DrawableObject.h"
#include "Rendering/Model.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/Texture.h"
#include "Transformations/Transformation.h"

DrawableObject::DrawableObject(Model* model, ShaderProgram* program, Transformation* transformation) : model(model), program(program), transformation(transformation) {}

void DrawableObject::draw() const {
    program->setUniform("modelMatrix", transformation->getMatrix());
    program->setUniform("objectColor", color);
    material.apply(program);

    if (program->hasUniform("hasTexture")) {

        if (texture) {
            texture->bind(0);
            program->setUniform("textureUnit", 0);
            program->setUniform("hasTexture", 1);
        }
        else {
            program->setUniform("hasTexture", 0);
        }
    }

    program->use();
    glStencilFunc(GL_ALWAYS, static_cast<GLint>(id), 0xFF);
    model->draw();
}

const char* DrawableObject::kindName(Kind kind) {

    switch (kind) {
        case Kind::Ground: 
            return "ground";
        
        case Kind::Collectible: 
            return "collectible";
        
        case Kind::Moving: 
            return "moving";
        
        default: 
            return "normal";
    }
}

void DrawableObject::setColor(const glm::vec3& value) {
    color = value;
}

const glm::vec3& DrawableObject::getColor() const {
    return color;
}

void DrawableObject::setProgram(ShaderProgram* value) {
    program = value;
}

void DrawableObject::setMaterial(const Material& value) {
    material = value;
}

void DrawableObject::setTexture(Texture* value) {
    texture = value;
}

void DrawableObject::setId(unsigned value) {
    id = value;
}

unsigned DrawableObject::getId() const {
    return id;
}

void DrawableObject::setKind(Kind value) {
    kind = value;
}

DrawableObject::Kind DrawableObject::getKind() const {
    return kind;
}
