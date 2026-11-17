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

// Sdilene zdroje pro vsechny sceny. Kazdy zdroj se vytvori jen jednou
// (pri prvnim pozadavku) a dalsi sceny dostanou stejnou instanci.
// Vsechny typy se cachuji stejne: mapa klic -> unique_ptr.
// Klice z cest se normalizuji ("./a/b.png", "a\\b.png" i "a/b.png" je stejna textura).
// Vlastni ho Application, sceny si drzi jen ukazatele / reference.
// Zdroje zanikaji az s Resources (po scenach), takze ukazatele v objektech nikdy nevisi.
class Resources {
public:
    Resources();
    ~Resources();
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    // cesty relativne k assets/shaders/, napr. getShader("lit.vert", "phong.frag")
    ShaderProgram* getShader(const std::string& vertexFile, const std::string& fragmentFile);

    // vestavene modely (BasicModels.h): "skycube", "floor", "hudQuad"
    Model* getModel(const std::string& name);

    // OBJ pres Assimp, napr. getObjModel("assets/models/statue/statue.obj")
    // nacte se jen jednou, dalsi volani vrati stejny model (dily + textury + bounding box)
    // mergeColors = vsechny dily do jednoho modelu s barvami z .mtl (1 draw call, bez textur)
    const LoadedModel& getObjModel(const std::string& path, bool mergeColors = false);

    // obrazek, napr. getTexture("assets/textures/Default/default.png")
    // stejny obrazek s jinym nastavenim (napr. HUD bez mipmap) je samostatna textura
    Texture* getTexture(const std::string& path, const TextureOptions& options = TextureOptions());

    // slozka se 6 obrazky right/left/top/bottom/front/back + pripona
    CubeMap* getCubeMap(const std::string& directory, const std::string& extension = CubeMap::DEFAULT_EXTENSION);

    // skybox z cubemapy ve slozce (sdileny mezi scenami)
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
