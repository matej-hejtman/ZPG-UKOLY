// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Lit trees and bushes (night forest): like lit.vert (world position and normal for the
// lighting in the fragment shader), the color is made like in foliage.vert -
// vertices [0, woodVertexCount) are wood (brown), the rest are leaves (shades of green).

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
uniform int woodVertexCount;

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 vertexColor;
out vec2 texCoord;   // no UVs; declared because phong.frag reads it

// Pseudo-random number in <0, 1) from an integer (same input -> same output)
float hash(int n)
{
    return fract(sin(float(n) * 12.9898) * 43758.5453);
}

void main()
{
    vec4 wp = modelMatrix * vec4(position, 1.0);
    worldPosition = wp.xyz / wp.w;
    worldNormal = transpose(inverse(mat3(modelMatrix))) * normal;
    gl_Position = projectionMatrix * viewMatrix * wp;
    texCoord = vec2(0.0);

    // one random value per triangle, so a whole leaf has one color
    float variation = hash(gl_VertexID / 3);
    if (gl_VertexID < woodVertexCount)
        vertexColor = mix(vec3(0.30, 0.18, 0.08), vec3(0.45, 0.29, 0.14), variation);
    else
        vertexColor = mix(vec3(0.08, 0.32, 0.06), vec3(0.35, 0.68, 0.18), variation);
}
