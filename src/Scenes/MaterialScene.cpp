// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/MaterialScene.h"
#include "Core/Resources.h"
#include "Lighting/Material.h"
#include "Rendering/DrawableObject.h"
#include "Transformations/DynamicRotate.h"

#include <vector>

static glm::vec3 hexColor(unsigned rgb) {
    return glm::vec3((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF) / 255.0f;
}

static const float CUBE_SPACING = 1.8f;
static const float CUBE_HEIGHT = 1.0f;
static const float LIGHT_HEIGHT_ABOVE = 1.2f;
static const float ROTATION_SPEED = 45.0f;
static const glm::vec3 ROTATION_AXIS = glm::vec3(1.0f, 1.0f, 0.0f);
static const float LIGHT_INTENSITY = 2.5f;
static const glm::vec3 AMBIENT_LIGHT = glm::vec3(0.3f);

void MaterialScene::create() {
    setAmbientLight(AMBIENT_LIGHT);

    const std::vector<Material> materials = {
        Material::gold(), Material::silver(), Material::copper(), Material::bronze(), Material::chrome(),
        Material::pearl(), Material::emerald(), Material::ruby(), Material::plastic(), Material::rubber(),
    };

    const std::vector<glm::vec3> lightColors = {
        hexColor(0xF09D51), hexColor(0xECF0F1), hexColor(0x4C956C), hexColor(0x313638), hexColor(0xDD614A),
    };

    Model* cube = resources.getModel("cube");
    ShaderProgram* phong = resources.getShader("lit.vert", "phong.frag");
    Texture* texture = resources.getTexture(Texture::DEFAULT_PATH);

    const float rowZ[2] = { -4.0f, -7.0f };
    const float firstX = -0.5f * CUBE_SPACING * static_cast<float>(materials.size() - 1);

    for (int row = 0; row < 2; ++row) {
        for (std::size_t i = 0; i < materials.size(); ++i) {
            const glm::vec3 position(firstX + CUBE_SPACING * static_cast<float>(i), CUBE_HEIGHT, rowZ[row]);

            TransformationComposite* transformation = makeComposite();
            transformation->addStatic(Translate(position));
            transformation->addDynamic(make<DynamicRotate>(getClock(), ROTATION_SPEED, ROTATION_AXIS));

            DrawableObject* object = createObject(cube, phong, transformation);
            object->setMaterial(materials[i]);

            if (row == 0) {
                object->setColor(hexColor(0xECF0F1));
            } else {
                object->setTexture(texture);
            }

            Light* light = addLight();
            light->setPosition(position + glm::vec3(0.0f, LIGHT_HEIGHT_ABOVE, 0.0f));
            light->setColor(lightColors[(row * materials.size() + i) % lightColors.size()]);
            light->setAttenuation(1.0f, 0.35f, 0.44f);
            light->setIntensity(LIGHT_INTENSITY);
        }
    }
}
