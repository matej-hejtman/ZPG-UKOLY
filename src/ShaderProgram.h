// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include "ICameraObserver.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Camera;

class ShaderProgram : public ICameraObserver {
public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram() override;
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void use() const;
    static void unbind();

    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const glm::vec3& value);
    void setUniform(const std::string& name, const glm::mat4& value);

    bool hasUniform(const std::string& name) const;

    void onCameraChanged(Camera* camera) override;

private:
    GLint getUniformLocation(const std::string& name);

    GLuint id = 0;
    std::string description;
    std::unordered_set<std::string> missing;

    GLint lookupLocation(const std::string& name) const;
    mutable std::unordered_map<std::string, GLint> locationCache;
};
