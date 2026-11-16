// Author: Matěj Hejtman, login: HEJ0094
#include "Core/Resources.h"
#include "Lighting/LightBuffer.h"
#include "Rendering/BasicModels.h"
#include "Rendering/Model.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/CubeMap.h"
#include "Textures/Skybox.h"
#include "Textures/TextureManager.h"

#include "login.h"
#include "sphere.h"

#include <array>
#include <iostream>

static const std::string SHADER_DIR = "assets/shaders/";

Resources::Resources()
    : textures(std::make_unique<TextureManager>()), lightBuffer(std::make_unique<LightBuffer>()) {}

Resources::~Resources() = default;

ShaderProgram* Resources::getShader(const std::string& vertexFile, const std::string& fragmentFile) {
    const std::string key = vertexFile + "|" + fragmentFile;

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

    if (name == "sphere") {
        model = std::make_unique<Model>(sphere, sizeof(sphere) / sizeof(float));               // pozice + normala
    } else if (name == "skycube") {
        model = std::make_unique<Model>(skycube, sizeof(skycube) / sizeof(float), 3);           // jen pozice
    } else if (name == "floor") {
        model = std::make_unique<Model>(texturedFloor, sizeof(texturedFloor) / sizeof(float), 8); // pozice + normala + uv
    } else if (name == "login") {
        model = std::make_unique<Model>(login, sizeof(login) / sizeof(float), 9, 6);            // pozice + (normala) + barva
    } else if (name == "hudQuad") {
        model = std::make_unique<Model>(hudQuad, sizeof(hudQuad) / sizeof(float), 6, 3);        // pozice + barva
    }

    if (!model) {
        std::cerr << "Resources: unknown model '" << name << "'" << std::endl;
        return nullptr;
    }

    Model* pointer = model.get();
    models.emplace(name, std::move(model));
    return pointer;
}

Texture* Resources::getTexture(const std::string& path) {
    return textures->get(path);
}

CubeMap* Resources::getCubeMap(const std::string& directory) {
    auto found = cubeMaps.find(directory);
    if (found != cubeMaps.end()) {
        return found->second.get();
    }

    static const char* FACES[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    std::array<std::string, 6> paths;
    for (int i = 0; i < 6; ++i) {
        paths[i] = directory + "/" + FACES[i] + ".jpg";
    }

    auto cubeMap = std::make_unique<CubeMap>(paths);
    CubeMap* pointer = cubeMap.get();
    cubeMaps.emplace(directory, std::move(cubeMap));
    return pointer;
}

Skybox* Resources::getSkybox(const std::string& directory) {
    auto found = skyboxes.find(directory);
    if (found != skyboxes.end()) {
        return found->second.get();
    }

    auto skybox = std::make_unique<Skybox>(getModel("skycube"), getShader("skybox.vert", "skybox.frag"), getCubeMap(directory));
    Skybox* pointer = skybox.get();
    skyboxes.emplace(directory, std::move(skybox));
    return pointer;
}

LightBuffer* Resources::getLightBuffer() {
    return lightBuffer.get();
}
