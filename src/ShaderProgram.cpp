// Author: Matt Hejtman, login: HEJ0094
#include "ShaderProgram.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <iostream>
#include <vector>

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) : description(vertexPath + " + " + fragmentPath)
{
    Shader vertexShader(GL_VERTEX_SHADER, vertexPath);
    Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentPath);

    id = glCreateProgram();
    glAttachShader(id, vertexShader.getId());
    glAttachShader(id, fragmentShader.getId());
    glLinkProgram(id);

    GLint status = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLength = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength > 0 ? logLength : 1, '\0');
        glGetProgramInfoLog(id, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader program link error (" << description << "):\n" << log.data() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glDetachShader(id, vertexShader.getId());
    glDetachShader(id, fragmentShader.getId());
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::use() const {
    glUseProgram(id);
}

void ShaderProgram::unbind() {
    glUseProgram(0);
}

GLint ShaderProgram::lookupLocation(const std::string& name) const {
    auto found = locationCache.find(name);

    if (found != locationCache.end()) {
        return found->second;
    }

    const GLint location = glGetUniformLocation(id, name.c_str());
    locationCache.emplace(name, location);
    return location;
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    const GLint location = lookupLocation(name);

    if (location == -1 && missing.insert(name).second) {
        std::cerr << "Uniform " << name << " not found (" << description << ")" << std::endl;
    }

    return location;
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) {
        return;
    }

    use();
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) {
        return;
    }

    use();
    glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) {
        return;
    }

    use();
    glUniform3fv(location, 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) {
        return;
    } 

    use();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

bool ShaderProgram::hasUniform(const std::string& name) const {
    return lookupLocation(name) != -1;
}

void ShaderProgram::onCameraChanged(Camera* camera) {
    setUniform("viewMatrix", camera->getViewMatrix());
    setUniform("projectionMatrix", camera->getProjectionMatrix());
    if (hasUniform("cameraPosition")) {
        setUniform("cameraPosition", camera->getEye());
    }
}
