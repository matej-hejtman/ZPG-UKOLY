// Author: Matt Hejtman, login: HEJ0094
#version 330 core

in vec3 vertexColor; // normal of the vertex, components in <-1, 1>

// Object color from DrawableObject; white = plain normal colors
uniform vec3 objectColor = vec3(1.0);

out vec4 fragColor;

void main()
{
    // abs() so that negative normal components are not clamped to black
    fragColor = vec4(abs(vertexColor) * objectColor, 1.0);
}
