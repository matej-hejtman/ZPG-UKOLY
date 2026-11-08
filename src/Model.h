// Author: Matt Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <cstddef>

class Model {
public:
    Model(const float* data, std::size_t floatCount, int floatsPerVertex = 6, int attribute1Offset = 3, int colorOffset = -1);
    ~Model();
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw() const;

private:
    GLuint vbo = 0;
    GLuint vao = 0;
    GLsizei vertexCount = 0;
    bool hasUV = false;            
    bool hasColorAttribute = false;
};
