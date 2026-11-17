// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Textures/CubeMap.h"
#include "Textures/Texture.h"

#include <memory>
#include <string>
#include <unordered_map>

class LightBuffer;
class Model;
struct LoadedModel;
class ShaderProgram;
class Skybox;

class Resources {
public:
    Resources();
    ~Resources();
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;
    
    ShaderProgram* getShader(const std::string& vertexFile, const std::string& fragmentFile);

    Model* getModel(const std::string& name);

    const LoadedModel& getObjModel(const std::string& path, bool mergeColors = false);

    Texture* getTexture(const std::string& path, const TextureOptions& options = TextureOptions());

    CubeMap* getCubeMap(const std::string& directory, const std::string& extension = CubeMap::DEFAULT_EXTENSION);

    Skybox* getSkybox(const std::string& directory, const std::string& extension = CubeMap::DEFAULT_EXTENSION);

    LightBuffer* getLightBuffer();

private:
    static std::string normalize(const std::string& path);

    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::unordered_map<std::string, std::unique_ptr<LoadedModel>> objModels;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<CubeMap>> cubeMaps;
    std::unordered_map<std::string, std::unique_ptr<Skybox>> skyboxes;
    std::unique_ptr<LightBuffer> lightBuffer;
};
