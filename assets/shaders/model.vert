// Author: Matt Hejtman, login: HEJ0094
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;   // color or normal, depends on the model

uniform mat4 modelMatrix;
// Identity by default: a program that is not attached to any camera draws
// directly in normalized device coordinates (used for the login signature)
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 vertexColor;

void main()
{
    // read from the right: model -> world -> camera -> clip space
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
    vertexColor = color;
}
