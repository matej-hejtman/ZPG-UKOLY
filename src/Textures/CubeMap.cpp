// Author: Matěj Hejtman, login: HEJ0094
#include "Textures/CubeMap.h"

#include <stb_image.h>

#include <cstdlib>
#include <iostream>

CubeMap::CubeMap(const std::string& directory, const std::string& extension) {
    if (tryLoad(directory, extension)) {
        return;
    }

    const bool isDefault = directory == DEFAULT_DIRECTORY && extension == DEFAULT_EXTENSION;
    if (!isDefault) {
        std::cerr << "WARNING: cube map " << directory << " -> using default " << DEFAULT_DIRECTORY << std::endl;
        if (tryLoad(DEFAULT_DIRECTORY, DEFAULT_EXTENSION)) {
            return;
        }
    }

    std::cerr << "ERROR: unable to load default cube map " << DEFAULT_DIRECTORY << std::endl;
    std::exit(EXIT_FAILURE);
}

std::array<std::string, 6> CubeMap::facePaths(const std::string& directory, const std::string& extension) {
    static const char* FACES[] = { "right", "left", "top", "bottom", "front", "back" };
    std::array<std::string, 6> paths;
    for (int i = 0; i < 6; ++i) {
        paths[i] = directory + "/" + FACES[i] + extension;
    }
    return paths;
}

bool CubeMap::tryLoad(const std::string& directory, const std::string& extension) {
    const std::array<std::string, 6> paths = facePaths(directory, extension);
    std::array<unsigned char*, 6> pixels{};
    int size = 0;
    bool ok = true;

    // nejdriv nacist a zkontrolovat vsechny steny, teprve pak nahrat na GPU
    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < 6 && ok; ++i) {
        int width = 0, height = 0, channels = 0;
        pixels[i] = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);

        if (!pixels[i]) {
            std::cerr << "ERROR: cube map face " << paths[i] << ": " << stbi_failure_reason() << std::endl;
            ok = false;
        } else if (width != height) {
            std::cerr << "ERROR: cube map face " << paths[i] << " is not square (" << width << "x" << height << ")" << std::endl;
            ok = false;
        } else if (i > 0 && width != size) {
            std::cerr << "ERROR: cube map face " << paths[i] << " is " << width << "x" << height
                      << ", but the first face is " << size << "x" << size << " (all faces must be the same size)" << std::endl;
            ok = false;
        } else {
            size = width;
        }
    }

    if (ok) {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels[i]);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        faceSize = size;
    }

    for (unsigned char* face : pixels) {
        if (face) {
            stbi_image_free(face);
        }
    }

    return ok;
}

CubeMap::~CubeMap() {
    glDeleteTextures(1, &id);
}

void CubeMap::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

int CubeMap::getFaceSize() const {
    return faceSize;
}
