// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <cstddef>

// Pevne lokace atributu pro CELY engine (vsechny shadery je ctou stejne):
//   layout(location = 0) in vec3 position;
//   layout(location = 1) in vec3 normal;
//   layout(location = 2) in vec2 uv;
//   layout(location = 3) in vec3 color;
namespace VertexAttribute {
    constexpr GLuint Position = 0;
    constexpr GLuint Normal = 1;
    constexpr GLuint Uv = 2;
    constexpr GLuint Color = 3;
}

// Popis rozlozeni jednoho vrcholu v poli floatu. Pozice je vzdy na zacatku (offset 0),
// ostatni offsety jsou ve floatech, -1 = atribut v datech neni.
struct VertexLayout {
    int floatsPerVertex = 3;
    int normalOffset = -1;
    int uvOffset = -1;
    int colorOffset = -1;

    static VertexLayout position();               // x y z
    static VertexLayout positionNormal();         // x y z  nx ny nz
    static VertexLayout positionColor();          // x y z  r g b
    static VertexLayout positionNormalUv();       // x y z  nx ny nz  u v
    static VertexLayout positionNormalColor();    // x y z  nx ny nz  r g b
    static VertexLayout positionNormalUvColor();  // x y z  nx ny nz  u v  r g b
};

// Geometrie na GPU (VBO + VAO). Chybejici atributy dostanou vychozi hodnotu:
// normala (0, 1, 0), UV (0, 0), barva bila.
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
