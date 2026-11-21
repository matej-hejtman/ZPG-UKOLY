// Author: Matěj Hejtman, login: HEJ0094
#include "Core/Resources.h"
#include "Lighting/LightBuffer.h"
#include "Rendering/BasicModels.h"
#include "forest_models/forest_all.h"
#include "space_models/space_all.h"
#include "Rendering/Model.h"
#include "Rendering/ModelLoader.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/Skybox.h"
#include "text_models/hej0094.h"
#include "zpg_models/clockModel.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace {

struct ModelSource {
    const float* data;
    std::size_t floatCount;
    VertexLayout layout;
};

template <std::size_t N>
ModelSource source(const float (&data)[N], const VertexLayout& layout = VertexLayout::positionNormalUv()) {
    return { data, N, layout };
}

ModelSource source(const std::vector<float>& data, const VertexLayout& layout = VertexLayout::positionNormalUv()) {
    return { data.data(), data.size(), layout };
}

const std::vector<float> sphereData = BasicShapes::sphere();
const std::vector<float> torusData = BasicShapes::torus();

const std::unordered_map<std::string, ModelSource> MODEL_SOURCES = {
    // --- zakladni ---
    { "skycube",           source(skycube, VertexLayout::position()) },
    { "floor",             source(texturedFloor) },
    { "cube",              source(cube) },
    { "sphere",            source(sphereData) },
    { "torus",             source(torusData) },
    { "hudQuad",           source(hudQuad, VertexLayout::positionColor()) },
    { "triangle",          source(triangle, VertexLayout::positionColor()) },
    { "square",            source(square, VertexLayout::positionColor()) },
    { "login",             source(hej0094) },

    // --- les: stromy ---
    { "tree_pine",         source(tree_pine) },
    { "tree_oak",          source(tree_oak) },
    { "tree_birch",        source(tree_birch) },
    { "dead_tree",         source(dead_tree) },
    { "sapling_pine",      source(sapling_pine) },
    // --- les: kere a kvetiny ---
    { "bush_round",        source(bush_round) },
    { "bush_berry",        source(bush_berry) },
    { "bush_fern",         source(bush_fern) },
    { "flower_daisy",      source(flower_daisy) },
    { "flower_tulip",      source(flower_tulip) },
    { "flower_bluebell",   source(flower_bluebell) },
    { "flower_poppy",      source(flower_poppy) },
    { "flower_lavender",   source(flower_lavender) },
    { "grass_tuft",        source(grass_tuft) },
    { "tall_grass",        source(tall_grass) },
    { "cattails",          source(cattails) },
    { "leaf_pile",         source(leaf_pile) },
    { "lily_pads",         source(lily_pads) },
    // --- les: kameny ---
    { "rock_small",        source(rock_small) },
    { "rock_medium",       source(rock_medium) },
    { "rock_boulder",      source(rock_boulder) },
    { "rock_mossy",        source(rock_mossy) },
    { "rock_cluster",      source(rock_cluster) },
    { "pebbles",           source(pebbles) },
    { "stepping_stones",   source(stepping_stones) },
    // --- les: drevo a houby ---
    { "tree_stump",        source(tree_stump) },
    { "stump_with_axe",    source(stump_with_axe) },
    { "log_fallen",        source(log_fallen) },
    { "log_mossy",         source(log_mossy) },
    { "branch",            source(branch) },
    { "firewood_pile",     source(firewood_pile) },
    { "pine_cone",         source(pine_cone) },
    { "mushroom_red",      source(mushroom_red) },
    { "mushroom_brown",    source(mushroom_brown) },
    { "mushroom_cluster",  source(mushroom_cluster) },
    // --- les: kemp a okoli ---
    { "campfire",          source(campfire) },
    { "fence",             source(fence) },
    { "signpost",          source(signpost) },
    { "bench",             source(bench) },
    { "wooden_bridge",     source(wooden_bridge) },
    { "tent",              source(tent) },
    { "barrel",            source(barrel) },
    { "birdhouse",         source(birdhouse) },
    { "ruin_pillar",       source(ruin_pillar) },
    { "terrain",           source(terrain) },
    { "water",             source(water) },

    // --- vesmir: hvezda a planety ---
    { "sun",               source(sun) },
    { "planet_mercury",    source(planet_mercury) },
    { "planet_venus",      source(planet_venus) },
    { "planet_earth",      source(planet_earth) },
    { "planet_mars",       source(planet_mars) },
    { "planet_jupiter",    source(planet_jupiter) },
    { "planet_saturn",     source(planet_saturn) },
    { "planet_uranus",     source(planet_uranus) },
    { "planet_neptune",    source(planet_neptune) },
    { "planet_pluto",      source(planet_pluto) },
    { "planet_alien",      source(planet_alien) },
    // --- vesmir: mesice ---
    { "moon_luna",         source(moon_luna) },
    { "moon_phobos",       source(moon_phobos) },
    { "moon_deimos",       source(moon_deimos) },
    { "moon_io",           source(moon_io) },
    { "moon_europa",       source(moon_europa) },
    { "moon_ganymede",     source(moon_ganymede) },
    { "moon_titan",        source(moon_titan) },
    // --- vesmir: asteroidy a komety ---
    { "asteroid_1",        source(asteroid_1) },
    { "asteroid_2",        source(asteroid_2) },
    { "asteroid_3",        source(asteroid_3) },
    { "asteroid_4",        source(asteroid_4) },
    { "asteroid_cluster",  source(asteroid_cluster) },
    { "asteroid_crystal",  source(asteroid_crystal) },
    { "comet",             source(comet) },
    { "meteor",            source(meteor) },
    // --- vesmir: ostatni ---
    { "ufo",               source(ufo) },
    { "alien",             source(alien) },
    { "rocket",            source(rocket) },
    { "satellite",         source(satellite) },
    { "space_station",     source(space_station) },
    { "astronaut",         source(astronaut) },
    { "moon_rover",        source(moon_rover) },
    { "black_hole",        source(black_hole) },
    { "monolith",          source(monolith) },
    { "star_field",        source(star_field) },
    { "orbit_ring",        source(orbit_ring) },
    // --- zpg ---
    { "clock",             source(clockModel)},
};

const ModelSource* findModelSource(const std::string& name) {
    auto found = MODEL_SOURCES.find(name);
    return found != MODEL_SOURCES.end() ? &found->second : nullptr;
}

}

static float lowestY(const float* data, std::size_t floatCount, std::size_t floatsPerVertex) {
    float minY = data[1];
    for (std::size_t i = 0; i + 1 < floatCount; i += floatsPerVertex) {
        minY = std::min(minY, data[i + 1]);
    }
    return minY;
}

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

    const ModelSource* src = findModelSource(name);
    if (!src) {
        std::cerr << "Resources: unknown model '" << name << "'" << std::endl;
        return nullptr;
    }

    auto model = std::make_unique<Model>(src->data, src->floatCount, src->layout);

    Model* pointer = model.get();
    models.emplace(name, std::move(model));
    return pointer;
}

float Resources::getModelBottom(const std::string& name) {
    auto found = modelBottoms.find(name);
    if (found != modelBottoms.end()) {
        return found->second;
    }

    float bottom = 0.0f;

    if (const ModelSource* src = findModelSource(name)) {
        bottom = lowestY(src->data, src->floatCount, static_cast<std::size_t>(src->layout.floatsPerVertex));
    } else {
        std::cerr << "Resources: unknown model bottom '" << name << "' -> 0" << std::endl;
    }

    modelBottoms.emplace(name, bottom);
    return bottom;
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
