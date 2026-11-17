// Author: Matej Hejtman, login: HEJ0094
#version 330 core

// HUD: bez kamery, jen ortho projekce (x: -aspect..aspect, y: -1..1)
// lokace atributu jako v celem enginu: 0 = pozice, 3 = barva
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 vertexColor;

void main() {
    vertexColor = color;
    gl_Position = projectionMatrix * modelMatrix * vec4(position, 1.0);
}
