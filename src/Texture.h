// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <string>

class Texture {
public:
    explicit Texture(const std::string& path);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    void bind(int unit) const;

private:
    GLuint id = 0;
};
