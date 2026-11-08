// Author: Matt Hejtman, login: HEJ0094
#version 330 core

in vec3 vertexColor;

// Object color from DrawableObject; white = only the vertex colors are visible
uniform vec3 objectColor = vec3(1.0);

out vec4 fragColor;

void main()
{
    fragColor = vec4(vertexColor * objectColor, 1.0);
}
