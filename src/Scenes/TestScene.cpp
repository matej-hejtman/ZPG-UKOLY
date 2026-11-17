// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/TestScene.h"

void TestScene::create() {
    getCamera()->setPosition(glm::vec3(0.0f), 90.0f, -90.0f);
    removeGround();
    setSkybox(nullptr);
    setClearColor(glm::vec3(0.0f));
}

void TestScene::update(float deltaTime) {
    (void)deltaTime;
}
