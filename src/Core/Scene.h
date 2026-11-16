// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Camera.h"
#include "Light.h"
#include "Rotate.h"
#include "Scale.h"
#include "TransformationComposite.h"
#include "Translate.h"
#include "LightBuffer.h"

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
    bool remove(DrawableObject* object);
    DrawableObject* getObjectById(unsigned id) const;

    Camera* getCamera() { 
        return &camera; 
    }

    Light* addLight() { 
        lights.push_back(std::make_unique<Light>());
		return lights.back().get();
	}

    const std::vector<std::unique_ptr<Light>>& getLights() const { 
        return lights; 
    }

    void addLitProgram(ShaderProgram* program);

    void setLightTransformation(Transformation* transformation) { 
        lightTransformation = transformation; 
    }

    void setSkybox(Skybox* value) { 
        skybox = value; 
    }

    void setClearColor(const glm::vec3& value) { 
        clearColor = value; 
    }

    const glm::vec3& getClearColor() const { 
        return clearColor; 
    }

    TransformationComposite* getBase() { 
        return &base; 
    }

    Translate& getMove() { 
        return controlMove; 
    }

    Rotate& getSpin() { 
        return controlSpin; 
    }

    Scale& getZoom() { 
        return controlZoom; 
    }

    void draw() const;

    void setLightBuffer(LightBuffer* buffer) { 
        lightBuffer = buffer; 
	}

private:
    std::vector<DrawableObject*> objects;
    std::vector<DrawableObject*> staticObjects;
    unsigned nextId = 1;
    std::vector<unsigned> freeIds;

    Camera camera;
    std::vector<std::unique_ptr<Light>> lights;
    std::vector<ShaderProgram*> litPrograms;
    Transformation* lightTransformation = nullptr;
    glm::vec3 clearColor = glm::vec3(0.0f);
    Skybox* skybox = nullptr;

    Translate controlMove{glm::vec3(0.0f)};
    Rotate controlSpin{0.0f, glm::vec3(0.0f, 1.0f, 0.0f)};
    Scale controlZoom{1.0f};
    TransformationComposite base;
    
	LightBuffer* lightBuffer = nullptr;
};
