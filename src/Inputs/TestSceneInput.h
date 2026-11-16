// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Inputs/SceneInput.h"

class TestScene;

// Input TestScene: co se stane po kliknuti.
// Drzi TestScene& (ne Scene&), aby mohl volat jeji metody bez pretypovani.
class TestSceneInput : public SceneInput {
public:
    explicit TestSceneInput(TestScene& scene);

    void onKey(GLFWwindow* window, int key, int action, int mods) override;

protected:
    void onHudClick(DrawableObject* object) override;
    void onGroundClick(DrawableObject* ground, const glm::vec3& position) override;
    void onObjectClick(DrawableObject* object, const glm::vec3& position) override;

private:
    TestScene& testScene;
};
