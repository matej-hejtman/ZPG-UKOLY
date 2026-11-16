// Author: Matěj Hejtman, login: HEJ0094
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

class CubeMap;
class LightBuffer;
class Model;
class ShaderProgram;
class Skybox;
class Texture;
class TextureManager;

// Sdilene zdroje pro vsechny sceny. Kazdy shader / model / textura se vytvori
// jen jednou (pri prvnim pozadavku) a dalsi sceny dostanou stejnou instanci.
// Vlastni ho Application, sceny si drzi jen referenci.
class Resources {
public:
    Resources();
    ~Resources();
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    // cesty relativne k assets/shaders/, napr. getShader("lit.vert", "phong.frag")
    ShaderProgram* getShader(const std::string& vertexFile, const std::string& fragmentFile);

    // vestavene modely: "sphere", "skycube", "floor", "login", "hudQuad"
    Model* getModel(const std::string& name);

    // cesta k obrazku, napr. getTexture("assets/textures/wooden_fence.png")
    Texture* getTexture(const std::string& path);

    // slozka se 6 obrazky posx/negx/posy/negy/posz/negz.jpg
    CubeMap* getCubeMap(const std::string& directory);

    // skybox z cubemapy ve slozce (sdileny mezi scenami)
    Skybox* getSkybox(const std::string& directory);

    LightBuffer* getLightBuffer();

private:
    std::unordered_map<std::string, std::unique_ptr<ShaderProgram>> shaders;
    std::unordered_map<std::string, std::unique_ptr<Model>> models;
    std::unordered_map<std::string, std::unique_ptr<CubeMap>> cubeMaps;
    std::unordered_map<std::string, std::unique_ptr<Skybox>> skyboxes;
    std::unique_ptr<TextureManager> textures;
    std::unique_ptr<LightBuffer> lightBuffer;
};
