// Author: Matěj Hejtman, login: HEJ0094
#pragma once

const float triangle[18] = {
    -0.9f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
    -0.1f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
};

const float square[36] = {
    0.1f, -0.4f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.9f, -0.4f, 0.0f,   0.0f, 1.0f, 0.0f,
    0.9f,  0.4f, 0.0f,   0.0f, 0.0f, 1.0f,

    0.1f, -0.4f, 0.0f,   1.0f, 0.0f, 0.0f,
    0.9f,  0.4f, 0.0f,   0.0f, 0.0f, 1.0f,
    0.1f,  0.4f, 0.0f,   1.0f, 1.0f, 0.0f,
};

const float ground[36] = {
    -1.0f, 0.0f,  1.0f,   0.10f, 0.30f, 0.08f,
     1.0f, 0.0f,  1.0f,   0.10f, 0.30f, 0.08f,
     1.0f, 0.0f, -1.0f,   0.30f, 0.55f, 0.22f,
    -1.0f, 0.0f,  1.0f,   0.10f, 0.30f, 0.08f,
     1.0f, 0.0f, -1.0f,   0.30f, 0.55f, 0.22f,
    -1.0f, 0.0f, -1.0f,   0.30f, 0.55f, 0.22f,
};

const float centeredTriangle[18] = {
    -0.15f, -0.1f, 0.0f,   1.0f, 0.0f, 0.0f,
     0.15f, -0.1f, 0.0f,   0.0f, 1.0f, 0.0f,
     0.0f,   0.2f, 0.0f,   0.0f, 0.0f, 1.0f,
};

const float skycube[108] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
};

const float texturedFloor[48] = {
    -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
     10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
     10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,

    -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
     10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f,
    -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
};

const float hudQuad[36] = {
    -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,
};

const float cube[288] = {
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
};

// ------ PROCEDURALNI TVARY ------
// Format stejny jako cube: pozice (3), normala (3), uv (2) -> VertexLayout::positionNormalUv()
// Trojuhelniky jsou CCW pri pohledu zvenku.

#include <cmath>
#include <vector>

namespace BasicShapes {

constexpr float PI = 3.14159265358979f;

inline void pushVertex(std::vector<float>& out, float px, float py, float pz,
                       float nx, float ny, float nz, float u, float v) {
    out.insert(out.end(), { px, py, pz, nx, ny, nz, u, v });
}

// Koule o polomeru 0.5 (stejna velikost jako cube), stacks = rovnobezky, slices = poledniky
inline std::vector<float> sphere(int stacks = 32, int slices = 64) {
    std::vector<float> out;
    out.reserve(static_cast<std::size_t>(stacks) * slices * 6 * 8);
    const float radius = 0.5f;

    auto vertex = [&](int i, int j) {
        const float theta = PI * i / stacks;          // 0 (nahore) .. PI (dole)
        const float phi = 2.0f * PI * j / slices;     // 0 .. 2PI
        const float nx = std::sin(theta) * std::cos(phi);
        const float ny = std::cos(theta);
        const float nz = std::sin(theta) * std::sin(phi);
        pushVertex(out, nx * radius, ny * radius, nz * radius, nx, ny, nz,
                   static_cast<float>(j) / slices, 1.0f - static_cast<float>(i) / stacks);
    };

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            vertex(i, j);     vertex(i + 1, j + 1); vertex(i + 1, j);
            vertex(i, j);     vertex(i, j + 1);     vertex(i + 1, j + 1);
        }
    }
    return out;
}

// Torus (kobliha): majorRadius = polomer prstence, minorRadius = polomer trubky
inline std::vector<float> torus(float majorRadius = 0.35f, float minorRadius = 0.15f,
                                int ringSegments = 64, int tubeSegments = 32) {
    std::vector<float> out;
    out.reserve(static_cast<std::size_t>(ringSegments) * tubeSegments * 6 * 8);

    auto vertex = [&](int i, int j) {
        const float u = 2.0f * PI * i / ringSegments; // kolem prstence
        const float v = 2.0f * PI * j / tubeSegments; // kolem trubky
        const float nx = std::cos(v) * std::cos(u);
        const float ny = std::sin(v);
        const float nz = std::cos(v) * std::sin(u);
        pushVertex(out,
                   majorRadius * std::cos(u) + minorRadius * nx,
                   minorRadius * ny,
                   majorRadius * std::sin(u) + minorRadius * nz,
                   nx, ny, nz,
                   static_cast<float>(i) / ringSegments, static_cast<float>(j) / tubeSegments);
    };

    for (int i = 0; i < ringSegments; ++i) {
        for (int j = 0; j < tubeSegments; ++j) {
            vertex(i, j); vertex(i + 1, j + 1); vertex(i + 1, j);
            vertex(i, j); vertex(i, j + 1);     vertex(i + 1, j + 1);
        }
    }
    return out;
}

} // namespace BasicShapes
