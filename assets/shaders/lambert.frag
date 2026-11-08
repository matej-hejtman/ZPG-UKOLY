// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Lambert: ambient + diffuse (Lambert's cosine law), material coefficients, attenuation

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;

struct Material
{
    vec3 ambient;     // r_a
    vec3 diffuse;     // r_d
    vec3 specular;    // r_s
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

void main()
{
    vec3 baseColor = vertexColor * objectColor;
    vec3 n = normalize(worldNormal);                     // interpolated normal is not unit length
    vec3 toLight = lightPosition - worldPosition;
    vec3 l = normalize(toLight);                         // from the surface to the light
    float att = attenuation(length(toLight));

    vec3 ambient = material.ambient * baseColor;         // not attenuated (light of the surroundings)
    vec3 diffuseColor = att * material.diffuse * diffuse(n, l) * baseColor * lightColor;
    fragColor = vec4(ambient + diffuseColor, 1.0);
}
