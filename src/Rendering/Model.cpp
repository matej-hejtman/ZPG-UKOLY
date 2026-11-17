// Author: Matěj Hejtman, login: HEJ0094
#include "Rendering/Model.h"

VertexLayout VertexLayout::position() {
    return { 3, -1, -1, -1 };
}

VertexLayout VertexLayout::positionNormal() {
    return { 6, 3, -1, -1 };
}

VertexLayout VertexLayout::positionColor() {
    return { 6, -1, -1, 3 };
}

VertexLayout VertexLayout::positionNormalUv() {
    return { 8, 3, 6, -1 };
}

VertexLayout VertexLayout::positionNormalColor() {
    return { 9, 3, -1, 6 };
}

VertexLayout VertexLayout::positionNormalUvColor() {
    return { 11, 3, 6, 8 };
}

static void enableAttribute(GLuint location, int components, GLsizei stride, int offsetFloats) {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsetFloats * sizeof(float)));
}

Model::Model(const float* data, std::size_t floatCount, const VertexLayout& layout) : vertexCount(static_cast<GLsizei>(floatCount / layout.floatsPerVertex)), hasNormal(layout.normalOffset >= 0), hasUV(layout.uvOffset >= 0), hasColor(layout.colorOffset >= 0) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(float), data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLsizei stride = layout.floatsPerVertex * sizeof(float);
    enableAttribute(VertexAttribute::Position, 3, stride, 0);

    if (hasNormal) {
        enableAttribute(VertexAttribute::Normal, 3, stride, layout.normalOffset);
    }

    if (hasUV) {
        enableAttribute(VertexAttribute::Uv, 2, stride, layout.uvOffset);
    }

    if (hasColor) {
        enableAttribute(VertexAttribute::Color, 3, stride, layout.colorOffset);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model::~Model() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Model::draw() const {
    glBindVertexArray(vao);

    if (!hasNormal) {
        glVertexAttrib3f(VertexAttribute::Normal, 0.0f, 1.0f, 0.0f);
    }

    if (!hasUV) {
        glVertexAttrib2f(VertexAttribute::Uv, 0.0f, 0.0f);
    }

    if (!hasColor) {
        glVertexAttrib3f(VertexAttribute::Color, 1.0f, 1.0f, 1.0f);
    }

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
