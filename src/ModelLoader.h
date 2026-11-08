// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Model;

struct ModelPart {
    std::unique_ptr<Model> model;
    std::string texturePath;
    glm::vec3 diffuseColor = glm::vec3(1.0f);
};

struct LoadedModel {
    std::vector<ModelPart> parts;
    glm::vec3 boundsMin = glm::vec3(0.0f);
    glm::vec3 boundsMax = glm::vec3(0.0f);
};

class ModelLoader {
public:
    static LoadedModel load(const std::string& path);
};
