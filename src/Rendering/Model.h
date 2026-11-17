// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <cstddef>

namespace VertexAttribute {
    constexpr GLuint Position = 0;
    constexpr GLuint Normal = 1;
    constexpr GLuint Uv = 2;
    constexpr GLuint Color = 3;
}

struct VertexLayout {
    int floatsPerVertex = 3;
    int normalOffset = -1;
    int uvOffset = -1;
    int colorOffset = -1;

    static VertexLayout position();
    static VertexLayout positionNormal();
    static VertexLayout positionColor();
    static VertexLayout positionNormalUv();
    static VertexLayout positionNormalColor();
    static VertexLayout positionNormalUvColor();
};

class Model {
public:
    Model(const float* data, std::size_t floatCount, const VertexLayout& layout);
    ~Model();
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    void draw() const;

private:
    GLuint vbo = 0;
    GLuint vao = 0;
    GLsizei vertexCount = 0;
    bool hasNormal = false;
    bool hasUV = false;
    bool hasColor = false;
};
