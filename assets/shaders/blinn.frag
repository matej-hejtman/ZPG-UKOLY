// Author: Matt Hejtman, login: HEJ0094
#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

struct Material
{
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

out vec4 fragColor;

// Attenuation of a point light with the distance d, kept in <0, 1>
float attenuation(float d)
{
    return clamp(1.0 / (lightConstant + lightLinear * d + lightQuadratic * d * d), 0.0, 1.0);
}

// Diffuse intensity: cosine of the angle between the normal and the direction to the light,
// zero when the light is behind the surface
float diffuse(vec3 n, vec3 l)
{
    return max(dot(n, l), 0.0);
}

// Specular intensity: how close the normal is to the half vector between light and camera
float specularBlinn(vec3 n, vec3 l, vec3 c)
{
    if (dot(n, l) < 0.0)
        return 0.0;                     // light behind the surface -> no highlight
    vec3 h = normalize(l + c);
    return pow(max(dot(n, h), 0.0), material.shininess);
}

void main()
{
    vec3 baseColor = vertexColor * objectColor;
    vec3 n = normalize(worldNormal);
    vec3 toLight = lightPosition - worldPosition;
    vec3 l = normalize(toLight);
    vec3 c = normalize(cameraPosition - worldPosition);
    float att = attenuation(length(toLight));

    vec3 ambient = material.ambient * baseColor;
    vec3 diffuseColor = att * material.diffuse * diffuse(n, l) * baseColor * lightColor;
    vec3 specularColor = att * material.specular * specularBlinn(n, l, c) * lightColor;
    fragColor = vec4(ambient + diffuseColor + specularColor, 1.0);
}
