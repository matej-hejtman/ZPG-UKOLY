// Author: Matt Hejtman, login: HEJ0094
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
uniform int woodVertexCount;

out vec3 vertexColor;

float hash(int n) {
    return fract(sin(float(n) * 12.9898) * 43758.5453);
}

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

    float shade = 0.65 + 0.35 * abs(normal.y);
    float variation = hash(gl_VertexID / 3);

    if (gl_VertexID < woodVertexCount) {
        vec3 darkBrown = vec3(0.30, 0.18, 0.08);
        vec3 lightBrown = vec3(0.45, 0.29, 0.14);
        vertexColor = mix(darkBrown, lightBrown, variation) * shade;
    }
    else {
        vec3 darkGreen = vec3(0.08, 0.32, 0.06);
        vec3 lightGreen = vec3(0.35, 0.68, 0.18);
        vertexColor = mix(darkGreen, lightGreen, variation) * shade;
    }
}
