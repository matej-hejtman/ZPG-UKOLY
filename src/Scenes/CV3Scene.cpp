// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/CV3Scene.h"
#include "Core/Resources.h"
#include "Rendering/DrawableObject.h"
#include "forest_models/terrain.h"
#include "Transformations/Translate.h"
#include "Transformations/Rotate.h"
#include "Transformations/Scale.h"
#include "Transformations/Transformation.h"

#include <cmath>
#include <random>
#include <string>

void CV3Scene::create() {

	shader = resources.getShader("lit.vert", "phong.frag");

    removeGround();

	Skybox* sb = resources.getSkybox("assets/textures/Skyboxes/skybox_forest_dawn");
    setSkybox(sb);

	DrawableObject* login = createObject(resources.getModel("login"), shader, makeComposite());
	login->setTexture(resources.getTexture("assets/models/text_models/hej0094_palette.png"));


	Light* light = addLight();
	light->setType(LightType::Directional);
	light->setDirection(glm::vec3(-1.f, -0.4f, -0.5f));
	light->setColor(glm::vec3(1.f, 0.7f, 0.5f));
	light->setIntensity(0.8f);

	setAmbientLight(glm::vec3(0.2f, 0.2f, 0.35f));

    OrganicM = Material ({
        glm::vec3(0.20f),
        glm::vec3(0.90f),
        glm::vec3(0.15f),
        16.0f,
        resources.getTexture("assets/models/forest_models/forest_palette.png")
    });

    WoodM = Material ({
        glm::vec3(0.20f),
        glm::vec3(0.85f),
        glm::vec3(0.03f),
        4.0f,
        resources.getTexture("assets/models/forest_models/forest_palette.png")
    });

    MushroomM = Material ({
        glm::vec3(0.20f),
        glm::vec3(0.90f),
        glm::vec3(0.25f),
        24.0f,
        resources.getTexture("assets/models/forest_models/forest_palette.png")
    });

    WaterM = Material ({
        glm::vec3(0.15f),
        glm::vec3(0.60f),
        glm::vec3(0.90f),
        128.0f,
        resources.getTexture("assets/models/forest_models/forest_palette.png")
    });

	glm::vec3 debugLocation(0.f, 5.f, 0.f);

	float n = 1.0f;

    buildTerrain();
    buildRocks();
    buildTrees();
    buildBushes();
}

void CV3Scene::update(float deltaTime) {
    (void)deltaTime;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distIntensity(-0.1f, 0.1f);


    campfireLight->setIntensity(glm::clamp(campfireLight->getIntensity() + distIntensity(gen), 0.f, 3.f));
}


void CV3Scene::buildTerrain() {
    TransformationComposite* t = makeComposite();
    t->addStatic(Scale(glm::vec3(25.f)));

    auto terrain = createObject(resources.getModel("terrain"), shader, t);
    terrain->setMaterial(OrganicM);

	TransformationComposite* waterT = makeComposite();
	waterT->addStatic(Translate(glm::vec3(10.f, -1.5f, 9.f)));
	waterT->addStatic(Scale(glm::vec3(7.f, 1.f, 7.f)));

    auto water = createObject(resources.getModel("water"), shader, waterT);
    water->setMaterial(WaterM);

    const float ruin_pillarX = -10.0f;
    const float ruin_pillarZ = -8.0f;
    const float ruin_pillarSize = 1.0f;
    const float ruin_pillarY = terrainHeight(ruin_pillarX, ruin_pillarZ) - resources.getModelBottom("ruin_pillar") * ruin_pillarSize;

    TransformationComposite* ruin_pillarT = makeComposite();
    ruin_pillarT->addStatic(Translate(glm::vec3(ruin_pillarX, ruin_pillarY, ruin_pillarZ)));
    ruin_pillarT->addStatic(Rotate(0.0f, glm::vec3(0.f, 1.f, 0.f)));
    ruin_pillarT->addStatic(Scale(glm::vec3(ruin_pillarSize)));

    auto ruin_pillar = createObject(resources.getModel("ruin_pillar"), shader, ruin_pillarT);
    ruin_pillar->setMaterial(OrganicM);

    const float campfireX = 4.0f;
    const float campfireZ = 4.0f;
    const float campfireSize = 0.6f;
    const float campfireY = terrainHeight(campfireX, campfireZ) - resources.getModelBottom("campfire") * campfireSize;

    TransformationComposite* campfireT = makeComposite();
    campfireT->addStatic(Translate(glm::vec3(campfireX, campfireY, campfireZ)));
    campfireT->addStatic(Rotate(0.0f, glm::vec3(0.f, 1.f, 0.f)));
    campfireT->addStatic(Scale(glm::vec3(campfireSize)));

    auto campfire = createObject(resources.getModel("campfire"), shader, campfireT);
    campfire->setMaterial(WoodM);

    campfireLight = addLight();
    campfireLight->setType(LightType::Point);
    campfireLight->setColor(glm::vec3(1.f, 0.5f, 0.2f));
    campfireLight->setIntensity(1.0f);
    campfireLight->setAttenuation(1.f, 0.09f, 0.032f);
    campfireLight->getTransformation().addStatic(Translate(glm::vec3(campfireX, campfireY + 0.5f, campfireZ)));

    const float fenceX = 0.0f;
    const float fenceZ = 10.0f;
    const float fenceSize = 0.8f;
    const float fenceY = terrainHeight(fenceX, fenceZ) - resources.getModelBottom("fence") * fenceSize;

    TransformationComposite* fenceT = makeComposite();
    fenceT->addStatic(Translate(glm::vec3(fenceX, fenceY, fenceZ)));
    fenceT->addStatic(Rotate(0.0f, glm::vec3(0.f, 1.f, 0.f)));
    fenceT->addStatic(Scale(glm::vec3(fenceSize)));

    auto fence = createObject(resources.getModel("fence"), shader, fenceT);
    fence->setMaterial(WoodM);

    const float signpostX = 2.0f;
    const float signpostZ = 0.0f;
    const float signpostSize = 0.7f;
    const float signpostY = terrainHeight(signpostX, signpostZ) - resources.getModelBottom("signpost") * signpostSize;

    TransformationComposite* signpostT = makeComposite();
    signpostT->addStatic(Translate(glm::vec3(signpostX, signpostY, signpostZ)));
    signpostT->addStatic(Rotate(30.0f, glm::vec3(0.f, 1.f, 0.f)));
    signpostT->addStatic(Scale(glm::vec3(signpostSize)));

    auto signpost = createObject(resources.getModel("signpost"), shader, signpostT);
    signpost->setMaterial(WoodM);

    const float benchX = 3.0f;
    const float benchZ = 5.0f;
    const float benchSize = 0.6f;
    const float benchY = terrainHeight(benchX, benchZ) - resources.getModelBottom("bench") * benchSize;

    TransformationComposite* benchT = makeComposite();
    benchT->addStatic(Translate(glm::vec3(benchX, benchY, benchZ)));
    benchT->addStatic(Rotate(-60.0f, glm::vec3(0.f, 1.f, 0.f)));
    benchT->addStatic(Scale(glm::vec3(benchSize)));

    auto bench = createObject(resources.getModel("bench"), shader, benchT);
    bench->setMaterial(WoodM);

    const float wooden_bridgeX = 5.0f;
    const float wooden_bridgeZ = 9.0f;
    const float wooden_bridgeSize = 1.0f;
    const float wooden_bridgeY = terrainHeight(wooden_bridgeX, wooden_bridgeZ) - resources.getModelBottom("wooden_bridge") * wooden_bridgeSize + 0.3f;

    TransformationComposite* wooden_bridgeT = makeComposite();
    wooden_bridgeT->addStatic(Translate(glm::vec3(wooden_bridgeX, wooden_bridgeY, wooden_bridgeZ)));
    wooden_bridgeT->addStatic(Rotate(0.0f, glm::vec3(0.f, 1.f, 0.f)));
    wooden_bridgeT->addStatic(Scale(glm::vec3(wooden_bridgeSize)));

    auto wooden_bridge = createObject(resources.getModel("wooden_bridge"), shader, wooden_bridgeT);
    wooden_bridge->setMaterial(WoodM);

    const float tentX = 1.0f;
    const float tentZ = 8.0f;
    const float tentSize = 1.0f;
    const float tentY = terrainHeight(tentX, tentZ) - resources.getModelBottom("tent") * tentSize;

    TransformationComposite* tentT = makeComposite();
    tentT->addStatic(Translate(glm::vec3(tentX, tentY, tentZ)));
    tentT->addStatic(Rotate(20.0f, glm::vec3(0.f, 1.f, 0.f)));
    tentT->addStatic(Scale(glm::vec3(tentSize)));

    auto tent = createObject(resources.getModel("tent"), shader, tentT);
    tent->setMaterial(OrganicM);

    const float barrelX = 3.0f;
    const float barrelZ = 3.0f;
    const float barrelSize = 0.3f;
    const float barrelY = terrainHeight(barrelX, barrelZ) - resources.getModelBottom("barrel") * barrelSize;

    TransformationComposite* barrelT = makeComposite();
    barrelT->addStatic(Translate(glm::vec3(barrelX, barrelY, barrelZ)));
    barrelT->addStatic(Rotate(0.0f, glm::vec3(0.f, 1.f, 0.f)));
    barrelT->addStatic(Scale(glm::vec3(barrelSize)));

    auto barrel = createObject(resources.getModel("barrel"), shader, barrelT);
    barrel->setMaterial(WoodM);

    const float birdhouseX = -3.0f;
    const float birdhouseZ = -3.0f;
    const float birdhouseSize = 0.5f;
    const float birdhouseY = terrainHeight(birdhouseX, birdhouseZ) - resources.getModelBottom("birdhouse") * birdhouseSize;

    TransformationComposite* birdhouseT = makeComposite();
    birdhouseT->addStatic(Translate(glm::vec3(birdhouseX, birdhouseY, birdhouseZ)));
    birdhouseT->addStatic(Rotate(45.0f, glm::vec3(0.f, 1.f, 0.f)));
    birdhouseT->addStatic(Scale(glm::vec3(birdhouseSize)));

    auto birdhouse = createObject(resources.getModel("birdhouse"), shader, birdhouseT);
    birdhouse->setMaterial(WoodM);

    const float firewood_pileX = 3.0f;
    const float firewood_pileZ = 6.0f;
    const float firewood_pileSize = 0.5f;
    const float firewood_pileY = terrainHeight(firewood_pileX, firewood_pileZ) - resources.getModelBottom("firewood_pile") * firewood_pileSize;

    TransformationComposite* firewood_pileT = makeComposite();
    firewood_pileT->addStatic(Translate(glm::vec3(firewood_pileX, firewood_pileY, firewood_pileZ)));
    firewood_pileT->addStatic(Rotate(10.0f, glm::vec3(0.f, 1.f, 0.f)));
    firewood_pileT->addStatic(Scale(glm::vec3(firewood_pileSize)));

    auto firewood_pile = createObject(resources.getModel("firewood_pile"), shader, firewood_pileT);
    firewood_pile->setMaterial(WoodM);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrPos(-3, 3);
    std::uniform_int_distribution<> distrdeg(0, 179);

    for (int i = 0; i < 15; i++) {
	    TransformationComposite* lillyT = makeComposite();
	    lillyT->addStatic(Translate(glm::vec3(10.f, -1.4f, 9.f) + glm::vec3(distrPos(gen), 0.f, distrPos(gen))));
	    lillyT->addStatic(Rotate(static_cast<float>(distrdeg(gen)), glm::vec3(0.f, 1.f, 0.f)));

        auto lily_pads = createObject(resources.getModel("lily_pads"), shader, lillyT);
        lily_pads->setMaterial(OrganicM);
    }
}

void CV3Scene::buildRocks() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrPos(-25, 25);
    std::uniform_real_distribution<float> distrSiz(0.4f, 1.4f);
    std::uniform_int_distribution<> distrdeg(0, 179);
    std::uniform_int_distribution<> distrIdx(0, 6);

    for (int i = 0; i < 100; i++) {

        const char* name = nullptr;
        Material* material = nullptr;

        switch (distrIdx(gen)) {
        case 0: {
            name = "rock_small";
            material = &OrganicM;
            break;
        }
        case 1: {
            name = "rock_medium";
            material = &OrganicM;
            break;
        }
        case 2: {
            name = "rock_boulder";
            material = &OrganicM;
            break;
        }
        case 3: {
            name = "rock_mossy";
            material = &OrganicM;
            break;
        }
        case 4: {
            name = "rock_cluster";
            material = &OrganicM;
            break;
        }
        case 5: {
            name = "pebbles";
            material = &OrganicM;
            break;
        }
        case 6: {
            name = "stepping_stones";
            material = &OrganicM;
            break;
        }
        default:
            break;
        }

        if (name == nullptr) {
            continue;
        }

        float terrainX, terrainZ;

        do {
            terrainX = static_cast<float>(distrPos(gen));
            terrainZ = static_cast<float>(distrPos(gen));
        } while (isInCamp(terrainX, terrainZ));

        const float size = distrSiz(gen);

        const float bottom = resources.getModelBottom(name);
        const float terrainY = terrainHeight(terrainX, terrainZ) - bottom * size;

        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(glm::vec3(terrainX, terrainY, terrainZ)));
        t->addStatic(Rotate(static_cast<float>(distrdeg(gen)), glm::vec3(0.f, 1.f, 0.f)));
        t->addStatic(Scale(glm::vec3(size)));

        auto object = createObject(resources.getModel(name), shader, t);
        object->setMaterial(*material);
    }
}

void CV3Scene::buildTrees() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrPos(-24, 24);
    std::uniform_real_distribution<float> distrSiz(1.f, 1.5f);
    std::uniform_int_distribution<> distrdeg(0, 179);
    std::uniform_int_distribution<> distrIdx(0, 4);

    for (int i = 0; i < 250; i++) {

        TransformationComposite* t = makeComposite();

        float terrainX, terrainZ;

        do {
            terrainX = static_cast<float>(distrPos(gen));
            terrainZ = static_cast<float>(distrPos(gen));
        } while (isInCamp(terrainX, terrainZ) || isInWater(terrainX, terrainZ));

        t->addStatic(Translate(glm::vec3(terrainX, terrainHeight(terrainX, terrainZ) + 1.f, terrainZ)));
        t->addStatic(Rotate(static_cast<float>(distrdeg(gen)), glm::vec3(0.f, 1.f, 0.f)));

        switch (distrIdx(gen)) {
        case 0: {
            auto dead_tree = createObject(resources.getModel("dead_tree"), shader, t);
            dead_tree->setMaterial(WoodM);
            t->addStatic(Scale(glm::vec3(distrSiz(gen))));
            break;
        }
        case 1: {
            auto sapling_pine = createObject(resources.getModel("sapling_pine"), shader, t);
            sapling_pine->setMaterial(OrganicM);
            t->addStatic(Scale(glm::vec3(distrSiz(gen)) * 0.8f));
            break;
        }
        case 2: {
            auto tree_pine = createObject(resources.getModel("tree_pine"), shader, t);
            tree_pine->setMaterial(OrganicM);
            t->addStatic(Scale(glm::vec3(distrSiz(gen)) * 2.f));
            break;
        }
        case 3: {
            auto tree_oak = createObject(resources.getModel("tree_oak"), shader, t);
            tree_oak->setMaterial(OrganicM);
            t->addStatic(Scale(glm::vec3(distrSiz(gen) * 1.5f)));
            break;
        }
        case 4: {
            auto tree_birch = createObject(resources.getModel("tree_birch"), shader, t);
            tree_birch->setMaterial(OrganicM);
            t->addStatic(Scale(glm::vec3(distrSiz(gen))));
            break;
        }
        default:
            break;
        }
    }
}

void CV3Scene::buildBushes() {

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrPos(-24, 24);
    std::uniform_real_distribution<float> distrSiz(0.1f, 0.5f);
    std::uniform_int_distribution<> distrdeg(0, 179);
    std::uniform_int_distribution<> distrIdx(0, 20);

    const float size = distrSiz(gen);

    for (int i = 0; i < 500; i++) {

        const char* name = nullptr;
        Material* material = nullptr;

        switch (distrIdx(gen)) {
        case 0: {
            name = "leaf_pile";
            material = &OrganicM;
            break;
        }
        case 1: {
            name = "mushroom_red";
            material = &MushroomM;
            break;
        }
        case 2: {
            name = "bush_round";
            material = &OrganicM;
            break;
        }
        case 3: {
            name = "bush_berry";
            material = &OrganicM;
            break;
        }
        case 4: {
            name = "bush_fern";
            material = &OrganicM;
            break;
        }
        case 5: {
            name = "flower_daisy";
            material = &OrganicM;
            break;
        }
        case 6: {
            name = "flower_tulip";
            material = &OrganicM;
            break;
        }
        case 7: {
            name = "flower_bluebell";
            material = &OrganicM;
            break;
        }
        case 8: {
            name = "flower_poppy";
            material = &OrganicM;
            break;
        }
        case 9: {
            name = "flower_lavender";
            material = &OrganicM;
            break;
        }
        case 10: {
            name = "tree_stump";
            material = &WoodM;
            break;
        }
        case 11: {
            name = "stump_with_axe";
            material = &WoodM;
            break;
        }
        case 12: {
            name = "log_fallen";
            material = &WoodM;
            break;
        }
        case 13: {
            name = "log_mossy";
            material = &WoodM;
            break;
        }
        case 14: {
            name = "branch";
            material = &WoodM;
            break;
        }
        case 15: {
            name = "mushroom_brown";
            material = &MushroomM;
            break;
        }
        case 16: {
            name = "mushroom_cluster";
            material = &MushroomM;
            break;
        }
        case 17: {
            name = "grass_tuft";
            material = &OrganicM;
            break;
        }
        case 18: {
            name = "tall_grass";
            material = &OrganicM;
            break;
        }
        case 19: {
            name = "cattails";
            material = &OrganicM;
            break;
        }
        case 20: {
            name = "pine_cone";
            material = &WoodM;
            break;
        }
        default:
            break;
        }

        if (name == nullptr) {
            continue;
        }

        float terrainX, terrainZ;
        do {
            terrainX = static_cast<float>(distrPos(gen));
            terrainZ = static_cast<float>(distrPos(gen));
        } while (isInCamp(terrainX, terrainZ) || isInWater(terrainX, terrainZ));

        const float bottom = resources.getModelBottom(name);
        const float terrainY = terrainHeight(terrainX, terrainZ) - bottom * size;

        TransformationComposite* t = makeComposite();
        t->addStatic(Translate(glm::vec3(terrainX, terrainY, terrainZ)));
        t->addStatic(Rotate(static_cast<float>(distrdeg(gen)), glm::vec3(0.f, 1.f, 0.f)));
        t->addStatic(Scale(glm::vec3(size)));

        auto object = createObject(resources.getModel(name), shader, t);
        object->setMaterial(*material);
    }
}

bool CV3Scene::isInCamp(float x, float z) const {
    const glm::vec2 campCenter(3.f, 5.f);
    const float campRadius = 7.f;
    return glm::distance(glm::vec2(x, z), campCenter) < campRadius;
}

bool CV3Scene::isInWater(float x, float z) const {
    const glm::vec2 campCenter(10.f, 9.f);
    const float campRadius = 7.f;
    return glm::distance(glm::vec2(x, z), campCenter) < campRadius;
}

float CV3Scene::terrainHeight(float x, float z) const {
    const float s = 25.f;
    const float px = x / s, pz = z / s;

    for (int i = 0; i < terrain_vertex_count; i += 3) {
        const float* a = &terrain[(i + 0) * 8];
        const float* b = &terrain[(i + 1) * 8];
        const float* c = &terrain[(i + 2) * 8];

        if (a[4] <= 0.f) continue;

        const float d =(b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2]);
        if (std::abs(d) < 1e-8f) continue;

        const float w1 = ((b[2] - c[2]) * (px - c[0]) + (c[0] - b[0]) * (pz - c[2])) / d;
        const float w2 = ((c[2] - a[2]) * (px - c[0]) + (a[0] - c[0]) * (pz - c[2])) / d;
        const float w3 = 1.f - w1 - w2;

        if (w1 >= 0.f && w2 >= 0.f && w3 >= 0.f) {
            return (w1 * a[1] + w2 * b[1] + w3 * c[1]) * s;
        }
    }
    return 0.f;
}