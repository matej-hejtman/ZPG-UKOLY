// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Sky cube around the camera. Only the rotation of the camera is used, so the sky
// moves with the camera and can never be reached.

layout(location = 0) in vec3 position;

uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);

out vec3 texCoord3;

void main()
{
    // The position on the unit cube is also the direction from its center -> cube map lookup
    texCoord3 = position;
    // mat3(viewMatrix) drops the translation (4th column), mat4() puts it back as 4x4
    gl_Position = projectionMatrix * mat4(mat3(viewMatrix)) * vec4(position, 1.0);
}
