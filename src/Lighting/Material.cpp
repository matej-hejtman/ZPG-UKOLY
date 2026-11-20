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

// ------ METALS ------

Material Material::gold() {
    return make(
        {               // Ambient
            0.24725f,
            0.1995f,
            0.0745f
        },
        {               // Diffuse
            0.75164f,
            0.60648f,
            0.22648f
        },
        {               // Specular
            0.628281f,
            0.555802f,
            0.366065f
        },

        51.2f           // Shininess
    );
}

Material Material::silver() {
    return make(
        {               // Ambient
            0.19225f,
            0.19225f,
            0.19225f
        },
        {               // Diffuse
            0.50754f,
            0.50754f,
            0.50754f
        },
        {               // Specular
            0.508273f,
            0.508273f,
            0.508273f
        },

        51.2f           // Shininess
    );
}

Material Material::copper() {
    return make(
        {               // Ambient
            0.19125f,
            0.0735f,
            0.0225f
        },
        {               // Diffuse
            0.7038f,
            0.27048f,
            0.0828f
        },
        {               // Specular
            0.256777f,
            0.137622f,
            0.086014f
        },

        12.8f           // Shininess
    );
}

Material Material::bronze() {
    return make(
        {               // Ambient
            0.2125f,
            0.1275f,
            0.054f
        },
        {               // Diffuse
            0.714f,
            0.4284f,
            0.18144f
        },
        {               // Specular
            0.393548f,
            0.271906f,
            0.166721f
        },

        25.6f           // Shininess
    );
}

Material Material::brass() {
    return make(
        {               // Ambient
            0.329412f,
            0.223529f,
            0.027451f
        },
        {               // Diffuse
            0.780392f,
            0.568627f,
            0.113725f
        },
        {               // Specular
            0.992157f,
            0.941176f,
            0.807843f
        },

        27.897f         // Shininess
    );
}

Material Material::chrome() {
    return make(
        {               // Ambient
            0.25f,
            0.25f,
            0.25f
        },
        {               // Diffuse
            0.4f,
            0.4f,
            0.4f
        },
        {               // Specular
            0.774597f,
            0.774597f,
            0.774597f
        },

        76.8f           // Shininess
    );
}

Material Material::pewter() {
    return make(
        {               // Ambient
            0.105882f,
            0.058824f,
            0.113725f
        },
        {               // Diffuse
            0.427451f,
            0.470588f,
            0.541176f
        },
        {               // Specular
            0.333333f,
            0.333333f,
            0.521569f
        },

        9.84615f        // Shininess
    );
}

// ------ POLISHED METALS ------

Material Material::polishedGold() {
    return make(
        {               // Ambient
            0.24725f,
            0.2245f,
            0.0645f
        },
        {               // Diffuse
            0.34615f,
            0.3143f,
            0.0903f
        },
        {               // Specular
            0.797357f,
            0.723991f,
            0.208006f
        },

        83.2f           // Shininess
    );
}

Material Material::polishedSilver() {
    return make(
        {               // Ambient
            0.23125f,
            0.23125f,
            0.23125f
        },
        {               // Diffuse
            0.2775f,
            0.2775f,
            0.2775f
        },
        {               // Specular
            0.773911f,
            0.773911f,
            0.773911f
        },

        89.6f           // Shininess
    );
}

Material Material::polishedCopper() {
    return make(
        {               // Ambient
            0.2295f,
            0.08825f,
            0.0275f
        },
        {               // Diffuse
            0.5508f,
            0.2118f,
            0.066f
        },
        {               // Specular
            0.580594f,
            0.223257f,
            0.0695701f
        },

        51.2f           // Shininess
    );
}

Material Material::polishedBronze() {
    return make(
        {               // Ambient
            0.25f,
            0.148f,
            0.06475f
        },
        {               // Diffuse
            0.4f,
            0.2368f,
            0.1036f
        },
        {               // Specular
            0.774597f,
            0.458561f,
            0.200621f
        },

        76.8f           // Shininess
    );
}

// ------ GEMS & STONES ------

Material Material::emerald() {
    return make(
        {               // Ambient
            0.0215f,
            0.1745f,
            0.0215f
        },
        {               // Diffuse
            0.07568f,
            0.61424f,
            0.07568f
        },
        {               // Specular
            0.633f,
            0.727811f,
            0.633f
        },

        76.8f           // Shininess
    );
}

Material Material::ruby() {
    return make(
        {               // Ambient
            0.1745f,
            0.01175f,
            0.01175f
        },
        {               // Diffuse
            0.61424f,
            0.04136f,
            0.04136f
        },
        {               // Specular
            0.727811f,
            0.626959f,
            0.626959f
        },

        76.8f           // Shininess
    );
}

Material Material::jade() {
    return make(
        {               // Ambient
            0.135f,
            0.2225f,
            0.1575f
        },
        {               // Diffuse
            0.54f,
            0.89f,
            0.63f
        },
        {               // Specular
            0.316228f,
            0.316228f,
            0.316228f
        },

        12.8f           // Shininess
    );
}

Material Material::obsidian() {
    return make(
        {               // Ambient
            0.05375f,
            0.05f,
            0.06625f
        },
        {               // Diffuse
            0.18275f,
            0.17f,
            0.22525f
        },
        {               // Specular
            0.332741f,
            0.328634f,
            0.346435f
        },

        38.4f           // Shininess
    );
}

Material Material::pearl() {
    return make(
        {               // Ambient
            0.25f,
            0.20725f,
            0.20725f
        },
        {               // Diffuse
            1.0f,
            0.829f,
            0.829f
        },
        {               // Specular
            0.296648f,
            0.296648f,
            0.296648f
        },

        11.264f         // Shininess
    );
}

Material Material::turquoise() {
    return make(
        {               // Ambient
            0.1f,
            0.18725f,
            0.1745f
        },
        {               // Diffuse
            0.396f,
            0.74151f,
            0.69102f
        },
        {               // Specular
            0.297254f,
            0.30829f,
            0.306678f
        },

        12.8f           // Shininess
    );
}

// ------ PLASTICS ------

Material Material::blackPlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.01f,
            0.01f,
            0.01f
        },
        {               // Specular
            0.5f,
            0.5f,
            0.5f
        },

        32.0f           // Shininess
    );
}

Material Material::whitePlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.55f,
            0.55f,
            0.55f
        },
        {               // Specular
            0.7f,
            0.7f,
            0.7f
        },

        32.0f           // Shininess
    );
}

Material Material::redPlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.5f,
            0.0f,
            0.0f
        },
        {               // Specular
            0.7f,
            0.6f,
            0.6f
        },

        32.0f           // Shininess
    );
}

Material Material::greenPlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.1f,
            0.35f,
            0.1f
        },
        {               // Specular
            0.45f,
            0.55f,
            0.45f
        },

        32.0f           // Shininess
    );
}

Material Material::cyanPlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.1f,
            0.06f
        },
        {               // Diffuse
            0.0f,
            0.50980392f,
            0.50980392f
        },
        {               // Specular
            0.50196078f,
            0.50196078f,
            0.50196078f
        },

        32.0f           // Shininess
    );
}

Material Material::yellowPlastic() {
    return make(
        {               // Ambient
            0.0f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.5f,
            0.5f,
            0.0f
        },
        {               // Specular
            0.6f,
            0.6f,
            0.5f
        },

        32.0f           // Shininess
    );
}

// ------ RUBBERS ------

Material Material::blackRubber() {
    return make(
        {               // Ambient
            0.02f,
            0.02f,
            0.02f
        },
        {               // Diffuse
            0.01f,
            0.01f,
            0.01f
        },
        {               // Specular
            0.4f,
            0.4f,
            0.4f
        },

        10.0f           // Shininess
    );
}

Material Material::whiteRubber() {
    return make(
        {               // Ambient
            0.05f,
            0.05f,
            0.05f
        },
        {               // Diffuse
            0.5f,
            0.5f,
            0.5f
        },
        {               // Specular
            0.7f,
            0.7f,
            0.7f
        },

        10.0f           // Shininess
    );
}

Material Material::redRubber() {
    return make(
        {               // Ambient
            0.05f,
            0.0f,
            0.0f
        },
        {               // Diffuse
            0.5f,
            0.4f,
            0.4f
        },
        {               // Specular
            0.7f,
            0.04f,
            0.04f
        },

        10.0f           // Shininess
    );
}

Material Material::greenRubber() {
    return make(
        {               // Ambient
            0.0f,
            0.05f,
            0.0f
        },
        {               // Diffuse
            0.4f,
            0.5f,
            0.4f
        },
        {               // Specular
            0.04f,
            0.7f,
            0.04f
        },

        10.0f           // Shininess
    );
}

Material Material::cyanRubber() {
    return make(
        {               // Ambient
            0.0f,
            0.05f,
            0.05f
        },
        {               // Diffuse
            0.4f,
            0.5f,
            0.5f
        },
        {               // Specular
            0.04f,
            0.7f,
            0.7f
        },

        10.0f           // Shininess
    );
}

Material Material::yellowRubber() {
    return make(
        {               // Ambient
            0.05f,
            0.05f,
            0.0f
        },
        {               // Diffuse
            0.5f,
            0.5f,
            0.4f
        },
        {               // Specular
            0.7f,
            0.7f,
            0.04f
        },

        10.0f           // Shininess
    );
}
