// Author: Matt Hejtman, login: HEJ0094
#version 330 core

// Phong lighting model: ambient + diffuse + specular (reflected light vector),
// material coefficients, attenuation, optional texture as the base color

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 vertexColor;
in vec2 texCoord;

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
uniform vec3 cameraPosition;
// Key O (exercise 7): when false, the highlight is computed also where the light is
// behind the surface - a visible error with a small shininess
uniform bool cutBackSpecular = true;
// Texture of the object (DrawableObject binds it to the unit given here)
uniform sampler2D textureUnit;
uniform int hasTexture = 0;
// Key U (exercise 8): show the texture coordinates as a color (red = u, green = v)
uniform int uvTest = 0;

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

// Specular intensity: how close the reflected light direction is to the direction to the camera
float specularPhong(vec3 n, vec3 l, vec3 c)
{
    if (cutBackSpecular && dot(n, l) < 0.0)
        return 0.0;                     // light behind the surface -> no highlight
    vec3 r = reflect(-l, n);            // reflect() expects the incoming direction (light -> surface)
    return pow(max(dot(c, r), 0.0), material.shininess);
}

void main()
{
    if (uvTest == 1)
    {
        fragColor = vec4(texCoord, 1.0, 1.0);
        return;
    }

    vec3 surfaceColor = hasTexture == 1 ? texture(textureUnit, texCoord).rgb : vertexColor;
    vec3 baseColor = surfaceColor * objectColor;
    vec3 n = normalize(worldNormal);
    vec3 toLight = lightPosition - worldPosition;
    vec3 l = normalize(toLight);                          // surface -> light
    vec3 c = normalize(cameraPosition - worldPosition);   // surface -> camera
    float att = attenuation(length(toLight));

    vec3 ambient = material.ambient * baseColor;
    vec3 diffuseColor = att * material.diffuse * diffuse(n, l) * baseColor * lightColor;
    vec3 specularColor = att * material.specular * specularPhong(n, l, c) * lightColor;
    fragColor = vec4(ambient + diffuseColor + specularColor, 1.0);
}
