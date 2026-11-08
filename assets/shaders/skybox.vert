// Author: Matt Hejtman, login: HEJ0094
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 texCoord3;

void main() {
    texCoord3 = position;
    gl_Position = projectionMatrix * mat4(mat3(viewMatrix)) * vec4(position, 1.0);
}
