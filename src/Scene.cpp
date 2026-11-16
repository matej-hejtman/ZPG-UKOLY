// Author: Matt Hejtman, login: HEJ0094
#include "Scene.h"
#include "DrawableObject.h"
#include "ShaderProgram.h"
#include "Skybox.h"

#include <algorithm>
#include <iostream>

Scene::Scene() {
    base.addDynamic(&controlMove);
    base.addDynamic(&controlSpin);
    base.addDynamic(&controlZoom);
}

void Scene::add(DrawableObject* object) {
    unsigned id = 0;
    if (!freeIds.empty()) {
        id = freeIds.back();
        freeIds.pop_back();
    } else if (nextId <= 255) {
        id = nextId++;
    } else {
        std::cerr << "WARNING: scene has more than 255 objects, new object is not selectable" << std::endl;
    }

    object->setId(id);
    objects.push_back(object);
}

bool Scene::remove(DrawableObject* object) {
    auto it = std::find(objects.begin(), objects.end(), object);
    
    if (it == objects.end()) {
        return false;
    }

    objects.erase(it);
    
    if (object->getId() != 0) {
        freeIds.push_back(object->getId());
    }
    
    object->setId(0);
    return true;
}

DrawableObject* Scene::getObjectById(unsigned id) const {
    if (id == 0) {
        return nullptr;
    }

    for (DrawableObject* object : objects) {
        if (object->getId() == id) {
            return object;
        }
    }
    return nullptr;
}

void Scene::addLitProgram(ShaderProgram* program) {
    litPrograms.push_back(program);
}

void Scene::addStatic(DrawableObject* object) {
    staticObjects.push_back(object);
}

void Scene::draw() const {
    if (skybox) {
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        skybox->draw();
    }

    const Transformation* placement = lightTransformation ? lightTransformation : &base;

    for (const auto& light : lights) {
        if (!light->isEnabled()) {
            continue;
        }

        const glm::vec4 lightPosition = placement->getMatrix() * glm::vec4(light->getPosition(), 1.0f);

        for (ShaderProgram* program : litPrograms) {
            program->setUniform("lightPosition", glm::vec3(lightPosition) / lightPosition.w);
            program->setUniform("lightColor", light->getColor());
            program->setUniform("lightConstant", light->getConstant());
            program->setUniform("lightLinear", light->getLinear());
            program->setUniform("lightQuadratic", light->getQuadratic());
        }
    }
    

    for (DrawableObject* object : staticObjects) {
        object->draw();
    }

    for (DrawableObject* object : objects) {
        object->draw();
    }

    ShaderProgram::unbind();
}
