// Author: Matt Hejtman, login: HEJ0094
#include "CubeMap.h"

#include <stb_image.h>

#include <cstdlib>
#include <iostream>

CubeMap::CubeMap(const std::array<std::string, 6>& paths) {
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    stbi_set_flip_vertically_on_load(false);
    for (int i = 0; i < 6; ++i) {
        int width = 0, height = 0, channels = 0;
        unsigned char* pixels = stbi_load(paths[i].c_str(), &width, &height, &channels, 4);
        
        if (!pixels) {
            std::cerr << "Unable to load cube map face " << paths[i] << ": " << stbi_failure_reason() << std::endl;
            std::exit(EXIT_FAILURE);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap() {
    glDeleteTextures(1, &id);
}

void CubeMap::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}
