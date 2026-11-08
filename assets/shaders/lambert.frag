// Author: Matt Hejtman, login: HEJ0094
#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

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

out vec4 fragColor;

float attenuation(float d) {
    return clamp(1.0 / (lightConstant + lightLinear * d + lightQuadratic * d * d), 0.0, 1.0);
}

float diffuse(vec3 n, vec3 l) {
    return max(dot(n, l), 0.0);
}

void main() {
    vec3 baseColor = vertexColor * objectColor;
    vec3 n = normalize(worldNormal);
    vec3 toLight = lightPosition - worldPosition;
    vec3 l = normalize(toLight);
    float att = attenuation(length(toLight));

    vec3 ambient = material.ambient * baseColor;
    vec3 diffuseColor = att * material.diffuse * diffuse(n, l) * baseColor * lightColor;
    fragColor = vec4(ambient + diffuseColor, 1.0);
}
