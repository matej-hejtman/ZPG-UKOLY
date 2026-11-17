// Author: Matěj Hejtman, login: HEJ0094
#include "Rendering/DrawableObject.h"
#include "Rendering/Model.h"
#include "Rendering/ShaderProgram.h"
#include "Transformations/Transformation.h"

DrawableObject::DrawableObject(Model* model, ShaderProgram* program, Transformation* transformation) : model(model), program(program), transformation(transformation) {}

void DrawableObject::draw() const {
    if (!visible) {
        return;
    }

    program->use();
    program->setUniform("modelMatrix", transformation->getMatrix());
    program->setUniform("objectColor", color);
    material.apply(program);

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

Model* DrawableObject::getModel() const {
    return model;
}

void DrawableObject::setProgram(ShaderProgram* value) {
    program = value;
}

ShaderProgram* DrawableObject::getProgram() const {
    return program;
}

void DrawableObject::setTransformation(Transformation* value) {
    transformation = value;
}

Transformation* DrawableObject::getTransformation() const {
    return transformation;
}

void DrawableObject::setMaterial(const Material& value) {
    material = value;
}

const Material& DrawableObject::getMaterial() const {
    return material;
}

void DrawableObject::setTexture(Texture* value) {
    material.texture = value;
}

Texture* DrawableObject::getTexture() const {
    return material.texture;
}

void DrawableObject::setVisible(bool value) {
    visible = value;
}

bool DrawableObject::isVisible() const {
    return visible;
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
