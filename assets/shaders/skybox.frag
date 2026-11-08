// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Sky color from the cube map, no lighting

in vec3 texCoord3;

uniform samplerCube skybox;

out vec4 fragColor;

void main()
{
    fragColor = texture(skybox, texCoord3);
}
