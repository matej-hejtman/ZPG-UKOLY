// Author: Matt Hejtman, login: HEJ0094
#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;
in vec2 texCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 objectColor = vec3(1.0);
uniform Material material;
uniform vec3 lightPosition;
uniform vec3 lightColor = vec3(1.0);
uniform float lightConstant = 1.0;
uniform float lightLinear = 0.09;
uniform float lightQuadratic = 0.032;
uniform vec3 cameraPosition;
uniform bool cutBackSpecular = true;
uniform sampler2D textureUnit;
uniform int hasTexture = 0;
uniform int uvTest = 0;

out vec4 fragColor;

float attenuation(float d) {
    return clamp(1.0 / (lightConstant + lightLinear * d + lightQuadratic * d * d), 0.0, 1.0);
}

float diffuse(vec3 n, vec3 l) {
    return max(dot(n, l), 0.0);
}

float specularPhong(vec3 n, vec3 l, vec3 c) {
    if (cutBackSpecular && dot(n, l) < 0.0) {
        return 0.0;
    }

    vec3 r = reflect(-l, n);
    return pow(max(dot(c, r), 0.0), material.shininess);
}

void main() {
    if (uvTest == 1) {
        fragColor = vec4(texCoord, 1.0, 1.0);
        return;
    }

    vec3 surfaceColor = hasTexture == 1 ? texture(textureUnit, texCoord).rgb : vertexColor;
    vec3 baseColor = surfaceColor * objectColor;
    vec3 n = normalize(worldNormal);
    vec3 toLight = lightPosition - worldPosition;
    vec3 l = normalize(toLight);
    vec3 c = normalize(cameraPosition - worldPosition);
    float att = attenuation(length(toLight));

    vec3 ambient = material.ambient * baseColor;
    vec3 diffuseColor = att * material.diffuse * diffuse(n, l) * baseColor * lightColor;
    vec3 specularColor = att * material.specular * specularPhong(n, l, c) * lightColor;
    fragColor = vec4(ambient + diffuseColor + specularColor, 1.0);
}
