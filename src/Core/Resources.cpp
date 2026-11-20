// Author: Matěj Hejtman, login: HEJ0094
#include "Core/Resources.h"
#include "Lighting/LightBuffer.h"
#include "Rendering/BasicModels.h"
#include "Rendering/Model.h"
#include "Rendering/ModelLoader.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/Skybox.h"

#include <filesystem>
#include <iostream>

static const std::string SHADER_DIR = "assets/shaders/";

Resources::Resources() : lightBuffer(std::make_unique<LightBuffer>()) {}

Resources::~Resources() = default;

std::string Resources::normalize(const std::string& path) {
    return std::filesystem::path(path).lexically_normal().generic_string();
}

ShaderProgram* Resources::getShader(const std::string& vertexFile, const std::string& fragmentFile) {
    const std::string key = normalize(vertexFile) + "|" + normalize(fragmentFile);

    auto found = shaders.find(key);
    if (found != shaders.end()) {
        return found->second.get();
    }

    auto program = std::make_unique<ShaderProgram>(SHADER_DIR + vertexFile, SHADER_DIR + fragmentFile);
    if (program->hasUniformBlock("LightBuffer")) {
        program->bindUniformBlock("LightBuffer", LIGHT_BINDING_POINT);
    }

    ShaderProgram* pointer = program.get();
    shaders.emplace(key, std::move(program));
    return pointer;
}

Model* Resources::getModel(const std::string& name) {
    auto found = models.find(name);
    if (found != models.end()) {
        return found->second.get();
    }

    std::unique_ptr<Model> model;

    if (name == "skycube") {
        model = std::make_unique<Model>(skycube, sizeof(skycube) / sizeof(float), VertexLayout::position());
    } else if (name == "floor") {
        model = std::make_unique<Model>(texturedFloor, sizeof(texturedFloor) / sizeof(float), VertexLayout::positionNormalUv());
    } else if (name == "cube") {
        model = std::make_unique<Model>(cube, sizeof(cube) / sizeof(float), VertexLayout::positionNormalUv());
    } else if (name == "sphere") {
        const std::vector<float> data = BasicShapes::sphere();
        model = std::make_unique<Model>(data.data(), data.size(), VertexLayout::positionNormalUv());
    } else if (name == "torus") {
        const std::vector<float> data = BasicShapes::torus();
        model = std::make_unique<Model>(data.data(), data.size(), VertexLayout::positionNormalUv());
    } else if (name == "hudQuad") {
        model = std::make_unique<Model>(hudQuad, sizeof(hudQuad) / sizeof(float), VertexLayout::positionColor());
    } else if (name == "triangle") {
        model = std::make_unique<Model>(triangle, sizeof(triangle) / sizeof(float), VertexLayout::positionColor());
    } else if (name == "square") {
        model = std::make_unique<Model>(square, sizeof(square) / sizeof(float), VertexLayout::positionColor());
    }

    if (!model) {
        std::cerr << "Resources: unknown model '" << name << "'" << std::endl;
        return nullptr;
    }

    Model* pointer = model.get();
    models.emplace(name, std::move(model));
    return pointer;
}

const LoadedModel& Resources::getObjModel(const std::string& path, bool mergeColors) {
    const std::string normalized = normalize(path);
    const std::string key = mergeColors ? normalized + "|merged" : normalized;

    auto found = objModels.find(key);
    if (found != objModels.end()) {
        return *found->second;
    }

    auto loaded = std::make_unique<LoadedModel>(ModelLoader::load(normalized, mergeColors));
    const LoadedModel& reference = *loaded;
    objModels.emplace(key, std::move(loaded));
    return reference;
}

Texture* Resources::getTexture(const std::string& path, const TextureOptions& options) {
    const std::string normalized = normalize(path);
    std::string key = normalized;
    if (!options.repeat) key += "|clamp";
    if (!options.mipmaps) key += "|nomip";

    auto found = textures.find(key);
    if (found != textures.end()) {
        return found->second.get();
    }

    auto texture = std::make_unique<Texture>(normalized, options);
    Texture* pointer = texture.get();
    textures.emplace(key, std::move(texture));
    return pointer;
}

CubeMap* Resources::getCubeMap(const std::string& directory, const std::string& extension) {
    const std::string normalized = normalize(directory);
    const std::string key = normalized + "|" + extension;

    auto found = cubeMaps.find(key);
    if (found != cubeMaps.end()) {
        return found->second.get();
    }

    auto cubeMap = std::make_unique<CubeMap>(normalized, extension);
    CubeMap* pointer = cubeMap.get();
    cubeMaps.emplace(key, std::move(cubeMap));
    return pointer;
}

Skybox* Resources::getSkybox(const std::string& directory, const std::string& extension) {
    const std::string normalized = normalize(directory);
    const std::string key = normalized + "|" + extension;

    auto found = skyboxes.find(key);
    if (found != skyboxes.end()) {
        return found->second.get();
    }

    auto skybox = std::make_unique<Skybox>(getModel("skycube"), getShader("skybox.vert", "skybox.frag"), getCubeMap(normalized, extension));
    Skybox* pointer = skybox.get();
    skyboxes.emplace(key, std::move(skybox));
    return pointer;
}

LightBuffer* Resources::getLightBuffer() {
    return lightBuffer.get();
}
