// Author: Matt Hejtman, login: HEJ0094
#include "Model.h"

Model::Model(const float* data, std::size_t floatCount, int floatsPerVertex, int attribute1Offset, int colorOffset) : vertexCount(static_cast<GLsizei>(floatCount / floatsPerVertex)), hasUV(floatsPerVertex == 8), hasColorAttribute(colorOffset >= 0) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(float), data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLsizei stride = floatsPerVertex * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    
    if (floatsPerVertex >= 6) {
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(attribute1Offset * sizeof(float)));
    }

    if (hasUV) {
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
    }

    if (hasColorAttribute) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(colorOffset * sizeof(float)));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model::~Model()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Model::draw() const
{
    glBindVertexArray(vao);
    if (!hasUV) {
        glVertexAttrib2f(2, 0.0f, 0.0f);
    }

    if (!hasColorAttribute) {
        glVertexAttrib3f(3, 1.0f, 1.0f, 1.0f);
    }

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}
