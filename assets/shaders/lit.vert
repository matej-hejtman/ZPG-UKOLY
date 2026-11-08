// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Common vertex shader of all lit programs (constant, Lambert, Phong, Blinn-Phong).
// Lighting is computed per fragment, so this shader only passes the world-space
// position and normal on.

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;      // texture coordinates, (0, 0) for models without UV
layout(location = 3) in vec3 color;   // vertex color, white for models without colors

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
// How the normal is converted to world space (key N, exercise 7 demonstration):
// 0 = not transformed (wrong), 1 = model matrix (wrong for non-uniform scale),
// 2 = normal matrix transpose(inverse(M)) (correct)
uniform int normalMode = 2;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 vertexColor;
out vec2 texCoord;

void main()
{
    vec4 wp = modelMatrix * vec4(position, 1.0);
    worldPosition = wp.xyz / wp.w;
    // Only the 3x3 part: a normal is a direction, it must not be translated
    mat3 model3 = mat3(modelMatrix);
    if (normalMode == 0)
        worldNormal = normal;                                  // ignores rotation and scale
    else if (normalMode == 1)
        worldNormal = model3 * normal;                         // not perpendicular after non-uniform scale
    else
        worldNormal = transpose(inverse(model3)) * normal;     // normal matrix (M^-1)^T
    vertexColor = color;
    texCoord = uv;
    gl_Position = projectionMatrix * viewMatrix * wp;
}
