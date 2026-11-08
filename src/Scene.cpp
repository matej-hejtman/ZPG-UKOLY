// Author: Matt Hejtman, login: HEJ0094
#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Skybox.h"

Scene::Scene() {
    base.addDynamic(&controlMove);
    base.addDynamic(&controlSpin);
    base.addDynamic(&controlZoom);
}

void Scene::add(DrawableObject* object) {
    objects.push_back(object);
}

void Scene::addLitProgram(ShaderProgram* program) {
    litPrograms.push_back(program);
}

void Scene::addStatic(DrawableObject* object) {
    staticObjects.push_back(object);
}

void Scene::draw() const
{
    if (skybox) {
        skybox->draw();
    }

    const Transformation* placement = lightTransformation ? lightTransformation : &base;
    const glm::vec4 lightPosition = placement->getMatrix() * glm::vec4(light.getPosition(), 1.0f);
    
    for (ShaderProgram* program : litPrograms) {
        program->setUniform("lightPosition", glm::vec3(lightPosition) / lightPosition.w);
        program->setUniform("lightColor", light.getColor());
        program->setUniform("lightConstant", light.getConstant());
        program->setUniform("lightLinear", light.getLinear());
        program->setUniform("lightQuadratic", light.getQuadratic());
    }

    for (DrawableObject* object : staticObjects) {
        object->draw();
    }

    for (DrawableObject* object : objects) {
        object->draw();
    }

    ShaderProgram::unbind();
}
