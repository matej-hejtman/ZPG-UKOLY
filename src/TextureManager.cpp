// Author: Matt Hejtman, login: HEJ0094
#include "TextureManager.h"
#include "Texture.h"

TextureManager::TextureManager() = default;
TextureManager::~TextureManager() = default;

Texture* TextureManager::get(const std::string& path) {
    auto found = textures.find(path);
    if (found != textures.end()) {
        return found->second.get();
    }

    auto texture = std::make_unique<Texture>(path);
    Texture* pointer = texture.get();
    textures.emplace(path, std::move(texture));
    return pointer;
}

void TextureManager::clear() {
    textures.clear();
}
