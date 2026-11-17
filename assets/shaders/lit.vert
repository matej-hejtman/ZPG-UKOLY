// Author: Matěj Hejtman, login: HEJ0094
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;

uniform mat4 modelMatrix;
uniform mat3 normalMatrix = mat3(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 worldPosition;
out vec3 worldNormal;
out vec3 vertexColor;
out vec2 texCoord;

void main() {
    vec4 wp = modelMatrix * vec4(position, 1.0);
    worldPosition = wp.xyz / wp.w;
    worldNormal = normalMatrix * normal;
    vertexColor = color;
    texCoord = uv;
    gl_Position = projectionMatrix * viewMatrix * wp;
}
