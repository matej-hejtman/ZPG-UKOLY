// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <memory>
#include <string>
#include <unordered_map>

class Texture;

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    Texture* get(const std::string& path);
    void clear();

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};
