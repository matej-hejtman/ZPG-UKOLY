// Author: Matej Hejtman, login: HEJ0094
#version 330 core

#define MAX_LIGHTS 64   // musi sedet s MAX_LIGHTS v LightBuffer.h

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

struct LightSource {
    vec4 position;
    vec4 color;
    vec4 attenuation;
    vec4 direction;
    vec4 params;
};

layout(std140) uniform LightBuffer {
    ivec4 lightInfo;
    vec4 ambientLight;
    LightSource lights[MAX_LIGHTS];
};

uniform vec3 objectColor = vec3(1.0);
uniform Material material;
uniform vec3 cameraPosition;
uniform sampler2D textureUnit;
uniform int hasTexture = 0;

out vec4 fragColor;

float attenuation(LightSource light, float d) {
    return clamp(1.0 / (light.attenuation.x + light.attenuation.y * d + light.attenuation.z * d * d), 0.0, 1.0);
}

float specularPhong(vec3 n, vec3 l, vec3 c) {
    if (dot(n, l) < 0.0) {
        return 0.0;
    }

    vec3 r = reflect(-l, n);
    return pow(max(dot(c, r), 0.0), material.shininess);
}

void main() {
    vec3 surfaceColor = hasTexture == 1 ? texture(textureUnit, texCoord).rgb : vertexColor;
    vec3 baseColor = surfaceColor * objectColor;
    vec3 n = normalize(worldNormal);
    vec3 c = normalize(cameraPosition - worldPosition);

    vec3 result = material.ambient * ambientLight.rgb * baseColor;

    int lightCount = min(lightInfo.x, MAX_LIGHTS);
    for (int i = 0; i < lightCount; i++) {
        int type = int(lights[i].params.x);
        vec3 lightColor = lights[i].color.rgb;

        vec3 l;
        float att = 1.0;

        if (type == 1) {
            l = normalize(-lights[i].direction.xyz);
        } else {
            vec3 toLight = lights[i].position.xyz - worldPosition;
            l = normalize(toLight);
            att = attenuation(lights[i], length(toLight));

            if (type == 2) {
                float spot = dot(normalize(lights[i].direction.xyz), -l);
                att *= smoothstep(lights[i].params.z, lights[i].params.y, spot);
            }
        }

        result += att * material.diffuse * max(dot(n, l), 0.0) * baseColor * lightColor;
        result += att * material.specular * specularPhong(n, l, c) * lightColor;
    }

    fragColor = vec4(result, 1.0);
}
