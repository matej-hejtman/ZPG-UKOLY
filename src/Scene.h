// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "Camera.h"
#include "Light.h"
#include "Rotate.h"
#include "Scale.h"
#include "TransformationComposite.h"
#include "Translate.h"

#include <vector>

class DrawableObject;
class ShaderProgram;
class Skybox;

class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void add(DrawableObject* object);
    void addStatic(DrawableObject* object);

    Camera* getCamera() { return &camera; }
    Light* getLight() { return &light; }
    void addLitProgram(ShaderProgram* program);
    void setLightTransformation(Transformation* transformation) { lightTransformation = transformation; }

    void setSkybox(Skybox* value) { skybox = value; }

    void setClearColor(const glm::vec3& value) { clearColor = value; }
    const glm::vec3& getClearColor() const { return clearColor; }
    TransformationComposite* getBase() { return &base; }
    Translate& getMove() { return controlMove; }
    Rotate& getSpin() { return controlSpin; }
    Scale& getZoom() { return controlZoom; }

    void draw() const;

private:
    std::vector<DrawableObject*> objects;
    std::vector<DrawableObject*> staticObjects;

    Camera camera;
    Light light;
    std::vector<ShaderProgram*> litPrograms;
    Transformation* lightTransformation = nullptr;
    glm::vec3 clearColor = glm::vec3(0.0f);
    Skybox* skybox = nullptr;

    Translate controlMove{glm::vec3(0.0f)};
    Rotate controlSpin{0.0f, glm::vec3(0.0f, 1.0f, 0.0f)};
    Scale controlZoom{1.0f};
    TransformationComposite base;
};
