// Author: Matt Hejtman, login: HEJ0094
#include "DrawableObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Transformation.h"

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
    model->draw();
}
