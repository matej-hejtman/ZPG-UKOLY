// Author: Matt Hejtman, login: HEJ0094
#include "Shader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

static std::string readFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Unable to open file " << path << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Shader::Shader(GLenum type, const std::string& path)
{
    const std::string source = readFile(path);
    const char* sourcePtr = source.c_str();

    id = glCreateShader(type);
    glShaderSource(id, 1, &sourcePtr, nullptr);
    glCompileShader(id);

    GLint status = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength > 0 ? logLength : 1, '\0');
        glGetShaderInfoLog(id, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader compile error in file " << path << ":\n" << log.data() << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

Shader::~Shader() {
    glDeleteShader(id);
}
