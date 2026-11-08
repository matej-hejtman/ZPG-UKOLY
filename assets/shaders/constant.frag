// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Constant shading: the object color only, no light and no normals

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

uniform vec3 objectColor = vec3(1.0);

out vec4 fragColor;

void main()
{
    fragColor = vec4(vertexColor * objectColor, 1.0);
}
