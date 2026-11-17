// Author: Matěj Hejtman, login: HEJ0094
#include "Lighting/Material.h"
#include "Rendering/ShaderProgram.h"
#include "Textures/Texture.h"

#include <algorithm>

void Material::apply(ShaderProgram* program) const {
    if (program->hasUniform("material.ambient")) {
        program->setUniform("material.ambient", ambient);
    }

    if (program->hasUniform("material.diffuse")) {
        program->setUniform("material.diffuse", diffuse);
    }

    if (program->hasUniform("material.specular")) {
        program->setUniform("material.specular", specular);
    }

    if (program->hasUniform("material.shininess")) {
        program->setUniform("material.shininess", std::max(shininess, 1.0f));
    }

    if (program->hasUniform("hasTexture")) {
        if (texture) {
            texture->bind(0);
            program->setUniform("textureUnit", 0);
            program->setUniform("hasTexture", 1);
        } else {
            program->setUniform("hasTexture", 0);
        }
    }
}

static Material make(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess) {
    Material material;
    material.ambient = ambient;
    material.diffuse = diffuse;
    material.specular = specular;
    material.shininess = shininess;
    return material;
}

Material Material::gold() {
    return make({ 0.24725f, 0.1995f, 0.0745f }, { 0.75164f, 0.60648f, 0.22648f }, { 0.628281f, 0.555802f, 0.366065f }, 51.2f);
}

Material Material::silver() {
    return make({ 0.19225f, 0.19225f, 0.19225f }, { 0.50754f, 0.50754f, 0.50754f }, { 0.508273f, 0.508273f, 0.508273f }, 51.2f);
}

Material Material::copper() {
    return make({ 0.19125f, 0.0735f, 0.0225f }, { 0.7038f, 0.27048f, 0.0828f }, { 0.256777f, 0.137622f, 0.086014f }, 12.8f);
}

Material Material::bronze() {
    return make({ 0.2125f, 0.1275f, 0.054f }, { 0.714f, 0.4284f, 0.18144f }, { 0.393548f, 0.271906f, 0.166721f }, 25.6f);
}

Material Material::chrome() {
    return make({ 0.25f, 0.25f, 0.25f }, { 0.4f, 0.4f, 0.4f }, { 0.774597f, 0.774597f, 0.774597f }, 76.8f);
}

Material Material::pearl() {
    return make({ 0.25f, 0.20725f, 0.20725f }, { 1.0f, 0.829f, 0.829f }, { 0.296648f, 0.296648f, 0.296648f }, 11.264f);
}

Material Material::emerald() {
    return make({ 0.0215f, 0.1745f, 0.0215f }, { 0.07568f, 0.61424f, 0.07568f }, { 0.633f, 0.727811f, 0.633f }, 76.8f);
}

Material Material::ruby() {
    return make({ 0.1745f, 0.01175f, 0.01175f }, { 0.61424f, 0.04136f, 0.04136f }, { 0.727811f, 0.626959f, 0.626959f }, 76.8f);
}

Material Material::plastic() {
    return make({ 0.0f, 0.0f, 0.0f }, { 0.55f, 0.55f, 0.55f }, { 0.7f, 0.7f, 0.7f }, 32.0f);
}

Material Material::rubber() {
    return make({ 0.05f, 0.05f, 0.05f }, { 0.5f, 0.5f, 0.5f }, { 0.7f, 0.7f, 0.7f }, 10.0f);
}
