// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/TestScene.h"
#include "Rendering/DrawableObject.h"
#include "Core/Resources.h"
#include "Transformations/GrowScale.h"

#include <iostream>

static const glm::vec3 BUTTON_ON = glm::vec3(1.0f, 0.85f, 0.2f);
static const glm::vec3 BUTTON_OFF = glm::vec3(0.35f);
static const glm::vec3 HIGHLIGHT = glm::vec3(1.0f, 0.3f, 0.3f);
static const float INSERTED_RADIUS = 0.3f;

void TestScene::create() {
    ShaderProgram* phong = resources.getShader("lit.vert", "phong.frag");

    // svetlo nad scenou
    light = addLight();
    light->setPosition(glm::vec3(2.0f, 4.0f, -1.0f));
    light->setAttenuation(1.0f, 0.0f, 0.0f);

    // koule pred kamerou, stoji na podlaze (polomer 1)
    TransformationComposite* transformation = makeComposite();
    transformation->addStatic(Translate(glm::vec3(0.0f, 1.0f, -4.0f)));
    sphere = createObject(resources.getModel("sphere"), phong, transformation);
    sphere->setColor(glm::vec3(0.35f, 0.6f, 1.0f));

    // HUD tlacitko vlevo nahore: zapne / vypne svetlo
    lightButton = createHudObject(resources.getModel("hudQuad"), HudAnchor::TopLeft, glm::vec2(0.12f, -0.12f), 0.15f);
    lightButton->setColor(BUTTON_ON);
}

void TestScene::update(float deltaTime) {
    (void)deltaTime;
}

DrawableObject* TestScene::getLightButton() const {
    return lightButton;
}

void TestScene::toggleLight() {
    light->setEnabled(!light->isEnabled());
    lightButton->setColor(light->isEnabled() ? BUTTON_ON : BUTTON_OFF);
    std::cout << "Light " << (light->isEnabled() ? "ON" : "OFF") << std::endl;
}

void TestScene::insertSphere(const glm::vec3& groundPosition) {
    // stred koule nad bodem kliknuti, aby lezela na podlaze; naroste za 1 s
    TransformationComposite* transformation = makeComposite();
    transformation->addStatic(Translate(groundPosition + glm::vec3(0.0f, INSERTED_RADIUS, 0.0f)));
    transformation->addDynamic(make<GrowScale>(INSERTED_RADIUS, 1.0f));

    DrawableObject* inserted = createObject(resources.getModel("sphere"), resources.getShader("lit.vert", "phong.frag"), transformation);
    inserted->setColor(glm::vec3(0.4f, 0.9f, 0.4f));
}

void TestScene::toggleHighlight(DrawableObject* object) {
    if (highlighted) {
        highlighted->setColor(highlightedColor);
    }

    if (highlighted == object) {
        highlighted = nullptr;
        return;
    }

    highlighted = object;
    highlightedColor = object->getColor();
    object->setColor(HIGHLIGHT);
}
