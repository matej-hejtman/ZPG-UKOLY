// Author: Matěj Hejtman, login: HEJ0094
#include "Textures/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstdlib>
#include <iostream>

Texture::Texture(const std::string& requestedPath, const TextureOptions& options) : path(requestedPath) {
    stbi_set_flip_vertically_on_load(true);
    int channels = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (!pixels && path != DEFAULT_PATH) {
        std::cerr << "WARNING: unable to load texture " << path << ": " << stbi_failure_reason()
                  << " -> using default " << DEFAULT_PATH << std::endl;
        path = DEFAULT_PATH;
        pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
    }

    if (!pixels) {
        std::cerr << "ERROR: unable to load default texture " << path << ": " << stbi_failure_reason() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    const GLint wrap = options.repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

    if (options.mipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

float Texture::getAspect() const {
    return height > 0 ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
}

const std::string& Texture::getPath() const {
    return path;
}
