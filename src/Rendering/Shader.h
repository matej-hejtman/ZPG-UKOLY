// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <string>

class Shader {
public:
    Shader(GLenum type, const std::string& path);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    GLuint getId() const;

private:
    GLuint id = 0;
};
