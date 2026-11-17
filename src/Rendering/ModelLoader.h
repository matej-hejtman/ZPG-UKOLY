// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Lighting/Material.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Model;

// Jeden dil modelu = jeden material z .mtl
struct ModelPart {
    std::unique_ptr<Model> model;
    std::string texturePath;                    // prazdna = dil nema texturu (map_Kd)
    glm::vec3 diffuseColor = glm::vec3(1.0f);   // Kd, pouzije se jako barva objektu bez textury
    Material material;                          // specular (Ks) a shininess (Ns) z .mtl, ambient zustava vychozi
};

struct LoadedModel {
    std::vector<ModelPart> parts;
    glm::vec3 boundsMin = glm::vec3(0.0f);
    glm::vec3 boundsMax = glm::vec3(0.0f);
};

// Nacteni OBJ pres Assimp. Pouziva se pres Resources::getObjModel (cache).
class ModelLoader {
public:
    // mergeColors = vsechny dily do jednoho modelu s barvou Kd ve vrcholech (1 draw call, bez textur)
    static LoadedModel load(const std::string& path, bool mergeColors = false);
};
