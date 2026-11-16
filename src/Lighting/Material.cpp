// Author: Matěj Hejtman, login: HEJ0094
#include "Lighting/Material.h"
#include "Rendering/ShaderProgram.h"

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
        program->setUniform("material.shininess", shininess);
    }
}
