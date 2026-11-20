#version 330 core
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 vertexColor;

void main() {
    vertexColor = color;
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}