/// Author: Matej Hejtman, login: HEJ0094
#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

uniform vec3 objectColor = vec3(1.0);
uniform sampler2D textureUnit;
uniform int hasTexture = 0;

out vec4 fragColor;

void main() {
    vec3 base = hasTexture == 1 ? texture(textureUnit, texCoord).rgb : vertexColor;
    fragColor = vec4(base * objectColor, 1.0);
}