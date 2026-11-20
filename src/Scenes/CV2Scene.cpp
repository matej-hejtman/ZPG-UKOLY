// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/CV2Scene.h"
#include "Core/Resources.h"
#include "Rendering/DrawableObject.h"
#include "Transformations/DynamicRotate.h"
#include "Transformations/Oscillate.h"

void CV2Scene::create() {
    getCamera()->setPosition(glm::vec3(0.0f, 0.0f, 3.0f), 90.0f, -90.0f);

    removeGround();
    setSkybox(nullptr);

    setClearColor(glm::vec3(0.192f, 0.211f, 0.219f));

	shader = resources.getShader("basic.vert", "basic.frag");

    buildTriangle();
    buildSquare();
}

void CV2Scene::update(float deltaTime) {
    (void)deltaTime;
}

void CV2Scene::buildTriangle() {
    const glm::vec3 center(-0.5f, 0.0f, 0.0f);

	TransformationComposite* t = makeComposite();
    t->addStatic(Translate(center));
	t->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.0f, 0.0f, 1.0f), -30));
    t->addStatic(Translate(-center));

    createObject(resources.getModel("triangle"), shader, t);
}

void CV2Scene::buildSquare() {

    const glm::vec3 center(0.5f, 0.0f, 0.0f);
    
    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(center));
    t->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f), 30));
    t->addStatic(Translate(-center));
    
    createObject(resources.getModel("square"), shader, t);
}