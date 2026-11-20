// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/TestScene.h"
#include "Core/Resources.h"
#include "Transformations/Oscillate.h"
#include "Transformations/GrowScale.h"
#include "Transformations/DynamicRotate.h"
#include "Transformations/LineMove.h"

#include "Lighting/Material.h"
#include "Rendering/DrawableObject.h"

void TestScene::create() {
    shader = resources.getShader("lit.vert", "phong.frag");

    getCamera()->setPosition(glm::vec3(4.f, 0.f, 8.f), 90.f, -90.f);
    removeGround();

    setSkybox(nullptr);
    setClearColor(glm::vec3(0.192f, 0.211f, 0.219f));

    buildMetalMaterials();
    buildPolishedMetalMaterials();
    buildPreciousMaterials();
    buildPlacticMaterials();
    buildRubberMaterials();
	buildTransformations();
}

void TestScene::update(float deltaTime) {
    (void)deltaTime;
}

void TestScene::buildMetalMaterials() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 boxesSpread = glm::vec3(2.f, 0.f, 0.f);

    Light* light = addLight();
	light->setColor(glm::vec3(1.f, 1.f, 1.f));
	light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    TransformationComposite* goldT = makeComposite();
    goldT->addDynamic(t);
    goldT->addStatic(Translate(boxesSpread));
	goldT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 0));

    TransformationComposite* silverT = makeComposite();
    silverT->addDynamic(t);
    silverT->addStatic(Translate(boxesSpread * 2.f));
    silverT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 30));
    
    TransformationComposite* copperT = makeComposite();
    copperT->addDynamic(t);
    copperT->addStatic(Translate(boxesSpread * 3.f));
    copperT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 60));
    
    TransformationComposite* bronzeT = makeComposite();
    bronzeT->addDynamic(t);
    bronzeT->addStatic(Translate(boxesSpread * 4.f));
    bronzeT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 90));

    TransformationComposite* brassT = makeComposite();
    brassT->addDynamic(t);
    brassT->addStatic(Translate(boxesSpread * 5.f));
    brassT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 120));

    TransformationComposite* chromeT = makeComposite();
    chromeT->addDynamic(t);
    chromeT->addStatic(Translate(boxesSpread * 6.f));
    chromeT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 150));

    TransformationComposite* pewterT = makeComposite();
    pewterT->addDynamic(t);
    pewterT->addStatic(Translate(boxesSpread * 7.f));
    pewterT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 180));

	TransformationComposite* floorT = makeComposite();
	floorT->addStatic(Translate(location + glm::vec3(8.f, -2.f, 0.f)));
	floorT->addStatic(Scale(glm::vec3(1.f, 0.2f, 0.2f)));

    auto goldBox = createObject(resources.getModel("torus"), shader, goldT);     // gold
    auto silverBox = createObject(resources.getModel("torus"), shader, silverT); // silver
    auto copperBox = createObject(resources.getModel("torus"), shader, copperT); // copper
    auto bronzeBox = createObject(resources.getModel("torus"), shader, bronzeT); // bronze
    auto brassBox = createObject(resources.getModel("torus"), shader, brassT);   // brass
    auto chromeBox = createObject(resources.getModel("torus"), shader, chromeT); // chrome
    auto pewterBox = createObject(resources.getModel("torus"), shader, pewterT); // pewter
	auto floor = createObject(resources.getModel("floor"), shader, floorT);      // floor

	goldBox->setMaterial(Material::gold());
    silverBox->setMaterial(Material::silver());
    copperBox->setMaterial(Material::copper());
    bronzeBox->setMaterial(Material::bronze());
    brassBox->setMaterial(Material::brass());
    chromeBox->setMaterial(Material::chrome());
    pewterBox->setMaterial(Material::pewter());
    floor->setMaterial(Material::whitePlastic());

}

void TestScene::buildPolishedMetalMaterials() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 5.f);
    glm::vec3 boxesSpread = glm::vec3(2.f, 0.f, 0.f);

    Light* light = addLight();
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    TransformationComposite* polishedGoldT = makeComposite();
    polishedGoldT->addDynamic(t);
    polishedGoldT->addStatic(Translate(boxesSpread));
    polishedGoldT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 0));

    TransformationComposite* polishedSilverT = makeComposite();
    polishedSilverT->addDynamic(t);
    polishedSilverT->addStatic(Translate(boxesSpread * 2.f));
    polishedSilverT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 30));

    TransformationComposite* polishedCopperT = makeComposite();
    polishedCopperT->addDynamic(t);
    polishedCopperT->addStatic(Translate(boxesSpread * 3.f));
    polishedCopperT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 60));

    TransformationComposite* polishedBronzeT = makeComposite();
    polishedBronzeT->addDynamic(t);
    polishedBronzeT->addStatic(Translate(boxesSpread * 4.f));
    polishedBronzeT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 90));

    TransformationComposite* floorT = makeComposite();
    floorT->addStatic(Translate(location + glm::vec3(8.f, -2.f, 0.f)));
    floorT->addStatic(Scale(glm::vec3(1.f, 0.2f, 0.2f)));

    auto polishedGoldBox = createObject(resources.getModel("torus"), shader, polishedGoldT);       // polished gold
    auto polishedSilverBox = createObject(resources.getModel("torus"), shader, polishedSilverT);   // polished silver
    auto polishedCopperBox = createObject(resources.getModel("torus"), shader, polishedCopperT);   // polished copper
    auto polishedBronzeBox = createObject(resources.getModel("torus"), shader, polishedBronzeT);   // polished bronze
    auto floor = createObject(resources.getModel("floor"), shader, floorT);                       // floor

    polishedGoldBox->setMaterial(Material::polishedGold());
    polishedSilverBox->setMaterial(Material::polishedSilver());
    polishedCopperBox->setMaterial(Material::polishedCopper());
    polishedBronzeBox->setMaterial(Material::polishedBronze());
    floor->setMaterial(Material::whitePlastic());
}

void TestScene::buildPreciousMaterials() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 10.f);
    glm::vec3 boxesSpread = glm::vec3(2.f, 0.f, 0.f);

    Light* light = addLight();
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    TransformationComposite* emeraldT = makeComposite();
    emeraldT->addDynamic(t);
    emeraldT->addStatic(Translate(boxesSpread));
    emeraldT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 0));

    TransformationComposite* rubyT = makeComposite();
    rubyT->addDynamic(t);
    rubyT->addStatic(Translate(boxesSpread * 2.f));
    rubyT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 30));

    TransformationComposite* jadeT = makeComposite();
    jadeT->addDynamic(t);
    jadeT->addStatic(Translate(boxesSpread * 3.f));
    jadeT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 60));

    TransformationComposite* obsidianT = makeComposite();
    obsidianT->addDynamic(t);
    obsidianT->addStatic(Translate(boxesSpread * 4.f));
    obsidianT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 90));

    TransformationComposite* pearlT = makeComposite();
    pearlT->addDynamic(t);
    pearlT->addStatic(Translate(boxesSpread * 5.f));
    pearlT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 120));

    TransformationComposite* turquoiseT = makeComposite();
    turquoiseT->addDynamic(t);
    turquoiseT->addStatic(Translate(boxesSpread * 6.f));
    turquoiseT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 150));

    TransformationComposite* floorT = makeComposite();
    floorT->addStatic(Translate(location + glm::vec3(8.f, -2.f, 0.f)));
    floorT->addStatic(Scale(glm::vec3(1.f, 0.2f, 0.2f)));

    auto emeraldBox = createObject(resources.getModel("torus"), shader, emeraldT);       // emerald
    auto rubyBox = createObject(resources.getModel("torus"), shader, rubyT);             // ruby
    auto jadeBox = createObject(resources.getModel("torus"), shader, jadeT);             // jade
    auto obsidianBox = createObject(resources.getModel("torus"), shader, obsidianT);     // obsidian
    auto pearlBox = createObject(resources.getModel("torus"), shader, pearlT);           // pearl
    auto turquoiseBox = createObject(resources.getModel("torus"), shader, turquoiseT);   // turquoise
    auto floor = createObject(resources.getModel("floor"), shader, floorT);             // floor

    emeraldBox->setMaterial(Material::emerald());
    rubyBox->setMaterial(Material::ruby());
    jadeBox->setMaterial(Material::jade());
    obsidianBox->setMaterial(Material::obsidian());
    pearlBox->setMaterial(Material::pearl());
    turquoiseBox->setMaterial(Material::turquoise());
    floor->setMaterial(Material::whitePlastic());
}

void TestScene::buildPlacticMaterials() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 15.f);
    glm::vec3 boxesSpread = glm::vec3(2.f, 0.f, 0.f);

    Light* light = addLight();
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    TransformationComposite* blackPlasticT = makeComposite();
    blackPlasticT->addDynamic(t);
    blackPlasticT->addStatic(Translate(boxesSpread));
    blackPlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 0));

    TransformationComposite* whitePlasticT = makeComposite();
    whitePlasticT->addDynamic(t);
    whitePlasticT->addStatic(Translate(boxesSpread * 2.f));
    whitePlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 30));

    TransformationComposite* redPlasticT = makeComposite();
    redPlasticT->addDynamic(t);
    redPlasticT->addStatic(Translate(boxesSpread * 3.f));
    redPlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 60));

    TransformationComposite* greenPlasticT = makeComposite();
    greenPlasticT->addDynamic(t);
    greenPlasticT->addStatic(Translate(boxesSpread * 4.f));
    greenPlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 90));

    TransformationComposite* cyanPlasticT = makeComposite();
    cyanPlasticT->addDynamic(t);
    cyanPlasticT->addStatic(Translate(boxesSpread * 5.f));
    cyanPlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 120));

    TransformationComposite* yellowPlasticT = makeComposite();
    yellowPlasticT->addDynamic(t);
    yellowPlasticT->addStatic(Translate(boxesSpread * 6.f));
    yellowPlasticT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 150));

    TransformationComposite* floorT = makeComposite();
    floorT->addStatic(Translate(location + glm::vec3(8.f, -2.f, 0.f)));
    floorT->addStatic(Scale(glm::vec3(1.f, 0.2f, 0.2f)));

    auto blackPlasticBox = createObject(resources.getModel("torus"), shader, blackPlasticT);     // black plastic
    auto whitePlasticBox = createObject(resources.getModel("torus"), shader, whitePlasticT);     // white plastic
    auto redPlasticBox = createObject(resources.getModel("torus"), shader, redPlasticT);         // red plastic
    auto greenPlasticBox = createObject(resources.getModel("torus"), shader, greenPlasticT);     // green plastic
    auto cyanPlasticBox = createObject(resources.getModel("torus"), shader, cyanPlasticT);       // cyan plastic
    auto yellowPlasticBox = createObject(resources.getModel("torus"), shader, yellowPlasticT);   // yellow plastic
    auto floor = createObject(resources.getModel("floor"), shader, floorT);                     // floor

    blackPlasticBox->setMaterial(Material::blackPlastic());
    whitePlasticBox->setMaterial(Material::whitePlastic());
    redPlasticBox->setMaterial(Material::redPlastic());
    greenPlasticBox->setMaterial(Material::greenPlastic());
    cyanPlasticBox->setMaterial(Material::cyanPlastic());
    yellowPlasticBox->setMaterial(Material::yellowPlastic());
    floor->setMaterial(Material::whitePlastic());
}

void TestScene::buildRubberMaterials() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 20.f);
    glm::vec3 boxesSpread = glm::vec3(2.f, 0.f, 0.f);

    Light* light = addLight();
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    TransformationComposite* blackRubberT = makeComposite();
    blackRubberT->addDynamic(t);
    blackRubberT->addStatic(Translate(boxesSpread));
    blackRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 0));

    TransformationComposite* whiteRubberT = makeComposite();
    whiteRubberT->addDynamic(t);
    whiteRubberT->addStatic(Translate(boxesSpread * 2.f));
    whiteRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 30));

    TransformationComposite* redRubberT = makeComposite();
    redRubberT->addDynamic(t);
    redRubberT->addStatic(Translate(boxesSpread * 3.f));
    redRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 60));

    TransformationComposite* greenRubberT = makeComposite();
    greenRubberT->addDynamic(t);
    greenRubberT->addStatic(Translate(boxesSpread * 4.f));
    greenRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 90));

    TransformationComposite* cyanRubberT = makeComposite();
    cyanRubberT->addDynamic(t);
    cyanRubberT->addStatic(Translate(boxesSpread * 5.f));
    cyanRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 120));

    TransformationComposite* yellowRubberT = makeComposite();
    yellowRubberT->addDynamic(t);
    yellowRubberT->addStatic(Translate(boxesSpread * 6.f));
    yellowRubberT->addDynamic(make<Oscillate>(getClock(), 30.0f, 0.5f, glm::vec3(1.f, 1.f, 0.f), 150));

    TransformationComposite* floorT = makeComposite();
    floorT->addStatic(Translate(location + glm::vec3(8.f, -2.f, 0.f)));
    floorT->addStatic(Scale(glm::vec3(1.f, 0.2f, 0.2f)));

    auto blackRubberBox = createObject(resources.getModel("torus"), shader, blackRubberT);     // black rubber
    auto whiteRubberBox = createObject(resources.getModel("torus"), shader, whiteRubberT);     // white rubber
    auto redRubberBox = createObject(resources.getModel("torus"), shader, redRubberT);         // red rubber
    auto greenRubberBox = createObject(resources.getModel("torus"), shader, greenRubberT);     // green rubber
    auto cyanRubberBox = createObject(resources.getModel("torus"), shader, cyanRubberT);       // cyan rubber
    auto yellowRubberBox = createObject(resources.getModel("torus"), shader, yellowRubberT);   // yellow rubber
    auto floor = createObject(resources.getModel("floor"), shader, floorT);                   // floor

    blackRubberBox->setMaterial(Material::blackRubber());
    whiteRubberBox->setMaterial(Material::whiteRubber());
    redRubberBox->setMaterial(Material::redRubber());
    greenRubberBox->setMaterial(Material::greenRubber());
    cyanRubberBox->setMaterial(Material::cyanRubber());
    yellowRubberBox->setMaterial(Material::yellowRubber());
    floor->setMaterial(Material::whiteRubber());
}

void TestScene::buildTransformations() {
    glm::vec3 location = glm::vec3(0.f, 0.f, 25.f);
    glm::vec3 boxesSpread = glm::vec3(5.f, 0.f, 0.f);

    Light* light = addLight();
    light->setColor(glm::vec3(1.f, 1.f, 1.f));
    light->setIntensity(2.0f);
    light->setPosition(glm::vec3(location + glm::vec3(8.f, 10.f, 0.f)));

    TransformationComposite* t = makeComposite();
    t->addStatic(Translate(location));

    // S R T
	TransformationComposite* SRT = makeComposite();
    SRT->addStatic(*t);
    SRT->addStatic(Translate(boxesSpread));

    SRT->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    SRT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    SRT->addStatic(Scale(glm::vec3(2.f)));

    // R S T
    TransformationComposite* RST = makeComposite();
    RST->addStatic(*t);
    RST->addStatic(Translate(boxesSpread * 2.f));

    RST->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    RST->addStatic(Scale(glm::vec3(2.f)));
    RST->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // S T R
    TransformationComposite* STR = makeComposite();
    STR->addStatic(*t);
    STR->addStatic(Translate(boxesSpread * 3.f));

    STR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    STR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    STR->addStatic(Scale(glm::vec3(2.f)));

    // T S R
    TransformationComposite* TSR = makeComposite();
    TSR->addStatic(*t);
    TSR->addStatic(Translate(boxesSpread * 4.f));

    TSR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    TSR->addStatic(Scale(glm::vec3(2.f)));
    TSR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // R T S
    TransformationComposite* RTS = makeComposite();
    RTS->addStatic(*t);
    RTS->addStatic(Translate(boxesSpread * 5.f));

    RTS->addStatic(Scale(glm::vec3(2.f)));
    RTS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    RTS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

	// T R S
    TransformationComposite* TRS = makeComposite();
    TRS->addStatic(*t);
    TRS->addStatic(Translate(boxesSpread * 6.f));

    TRS->addStatic(Scale(glm::vec3(2.f)));
    TRS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    TRS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // DS R T
    TransformationComposite* DSRT = makeComposite();
    DSRT->addStatic(*t);
    DSRT->addStatic(Translate(boxesSpread * 7.f));

    DSRT->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DSRT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DSRT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DR S T
    TransformationComposite* DRST = makeComposite();
    DRST->addStatic(*t);
    DRST->addStatic(Translate(boxesSpread * 8.f));

    DRST->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRST->addStatic(Scale(glm::vec3(2.f)));
    DRST->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DS T R
    TransformationComposite* DSTR = makeComposite();
    DSTR->addStatic(*t);
    DSTR->addStatic(Translate(boxesSpread * 9.f));

    DSTR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DSTR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DSTR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DT S R
    TransformationComposite* DTSR = makeComposite();
    DTSR->addStatic(*t);
    DTSR->addStatic(Translate(boxesSpread * 10.f));

    DTSR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DTSR->addStatic(Scale(glm::vec3(2.f)));
    DTSR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // DR T S
    TransformationComposite* DRTS = makeComposite();
    DRTS->addStatic(*t);
    DRTS->addStatic(Translate(boxesSpread * 11.f));

    DRTS->addStatic(Scale(glm::vec3(2.f)));
    DRTS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRTS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DT R S
    TransformationComposite* DTRS = makeComposite();
    DTRS->addStatic(*t);
    DTRS->addStatic(Translate(boxesSpread * 12.f));

    DTRS->addStatic(Scale(glm::vec3(2.f)));
    DTRS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DTRS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // S DR T
    TransformationComposite* SDRT = makeComposite();
    SDRT->addStatic(*t);
    SDRT->addStatic(Translate(boxesSpread * 13.f));

    SDRT->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    SDRT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    SDRT->addStatic(Scale(glm::vec3(2.f)));

    // R DS T
    TransformationComposite* RDST = makeComposite();
    RDST->addStatic(*t);
    RDST->addStatic(Translate(boxesSpread * 14.f));

    RDST->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    RDST->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    RDST->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // S DT R
    TransformationComposite* SDTR = makeComposite();
    SDTR->addStatic(*t);
    SDTR->addStatic(Translate(boxesSpread * 15.f));

    SDTR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    SDTR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    SDTR->addStatic(Scale(glm::vec3(2.f)));

    // T DS R
    TransformationComposite* TDSR = makeComposite();
    TDSR->addStatic(*t);
    TDSR->addStatic(Translate(boxesSpread * 16.f));

    TDSR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    TDSR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TDSR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // R DT S
    TransformationComposite* RDTS = makeComposite();
    RDTS->addStatic(*t);
    RDTS->addStatic(Translate(boxesSpread * 17.f));

    RDTS->addStatic(Scale(glm::vec3(2.f)));
    RDTS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    RDTS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // T DR S
    TransformationComposite* TDRS = makeComposite();
    TDRS->addStatic(*t);
    TDRS->addStatic(Translate(boxesSpread * 18.f));

    TDRS->addStatic(Scale(glm::vec3(2.f)));
    TDRS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TDRS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // S R DT
    TransformationComposite* SRDT = makeComposite();
    SRDT->addStatic(*t);
    SRDT->addStatic(Translate(boxesSpread * 19.f));

    SRDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    SRDT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    SRDT->addStatic(Scale(glm::vec3(2.f)));

    // R S DT
    TransformationComposite* RSDT = makeComposite();
    RSDT->addStatic(*t);
    RSDT->addStatic(Translate(boxesSpread * 20.f));

    RSDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    RSDT->addStatic(Scale(glm::vec3(2.f)));
    RSDT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // S T DR
    TransformationComposite* STDR = makeComposite();
    STDR->addStatic(*t);
    STDR->addStatic(Translate(boxesSpread * 21.f));

    STDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    STDR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    STDR->addStatic(Scale(glm::vec3(2.f)));

    // T S DR
    TransformationComposite* TSDR = makeComposite();
    TSDR->addStatic(*t);
    TSDR->addStatic(Translate(boxesSpread * 22.f));

    TSDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TSDR->addStatic(Scale(glm::vec3(2.f)));
    TSDR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // R T DS
    TransformationComposite* RTDS = makeComposite();
    RTDS->addStatic(*t);
    RTDS->addStatic(Translate(boxesSpread * 23.f));

    RTDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    RTDS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    RTDS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // T R DS
    TransformationComposite* TRDS = makeComposite();
    TRDS->addStatic(*t);
    TRDS->addStatic(Translate(boxesSpread * 24.f));

    TRDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TRDS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    TRDS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // DS DR T
    TransformationComposite* DSDRT = makeComposite();
    DSDRT->addStatic(*t);
    DSDRT->addStatic(Translate(boxesSpread * 25.f));

    DSDRT->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DSDRT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DSDRT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DR DS T
    TransformationComposite* DRDST = makeComposite();
    DRDST->addStatic(*t);
    DRDST->addStatic(Translate(boxesSpread * 26.f));

    DRDST->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRDST->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DRDST->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DS DT R
    TransformationComposite* DSDTR = makeComposite();
    DSDTR->addStatic(*t);
    DSDTR->addStatic(Translate(boxesSpread * 27.f));

    DSDTR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DSDTR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DSDTR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DT DS R
    TransformationComposite* DTDSR = makeComposite();
    DTDSR->addStatic(*t);
    DTDSR->addStatic(Translate(boxesSpread * 28.f));

    DTDSR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DTDSR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DTDSR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // DR DT S
    TransformationComposite* DRDTS = makeComposite();
    DRDTS->addStatic(*t);
    DRDTS->addStatic(Translate(boxesSpread * 29.f));

    DRDTS->addStatic(Scale(glm::vec3(2.f)));
    DRDTS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DRDTS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DT DR S
    TransformationComposite* DTDRS = makeComposite();
    DTDRS->addStatic(*t);
    DTDRS->addStatic(Translate(boxesSpread * 30.f));

    DTDRS->addStatic(Scale(glm::vec3(2.f)));
    DTDRS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DTDRS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // DS R DT
    TransformationComposite* DSRDT = makeComposite();
    DSRDT->addStatic(*t);
    DSRDT->addStatic(Translate(boxesSpread * 31.f));

    DSRDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DSRDT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DSRDT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DR S DT
    TransformationComposite* DRSDT = makeComposite();
    DRSDT->addStatic(*t);
    DRSDT->addStatic(Translate(boxesSpread * 32.f));

    DRSDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DRSDT->addStatic(Scale(glm::vec3(2.f)));
    DRSDT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DS T DR
    TransformationComposite* DSTDR = makeComposite();
    DSTDR->addStatic(*t);
    DSTDR->addStatic(Translate(boxesSpread * 33.f));

    DSTDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DSTDR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DSTDR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DT S DR
    TransformationComposite* DTSDR = makeComposite();
    DTSDR->addStatic(*t);
    DTSDR->addStatic(Translate(boxesSpread * 34.f));

    DTSDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DTSDR->addStatic(Scale(glm::vec3(2.f)));
    DTSDR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // DR T DS
    TransformationComposite* DRTDS = makeComposite();
    DRTDS->addStatic(*t);
    DRTDS->addStatic(Translate(boxesSpread * 35.f));

    DRTDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DRTDS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRTDS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DT R DS
    TransformationComposite* DTRDS = makeComposite();
    DTRDS->addStatic(*t);
    DTRDS->addStatic(Translate(boxesSpread * 36.f));

    DTRDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DTRDS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DTRDS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // S DR DT
    TransformationComposite* SDRDT = makeComposite();
    SDRDT->addStatic(*t);
    SDRDT->addStatic(Translate(boxesSpread * 37.f));

    SDRDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    SDRDT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    SDRDT->addStatic(Scale(glm::vec3(2.f)));

    // R DS DT
    TransformationComposite* RDSDT = makeComposite();
    RDSDT->addStatic(*t);
    RDSDT->addStatic(Translate(boxesSpread * 38.f));

    RDSDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    RDSDT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    RDSDT->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // S DT DR
    TransformationComposite* SDTDR = makeComposite();
    SDTDR->addStatic(*t);
    SDTDR->addStatic(Translate(boxesSpread * 39.f));

    SDTDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    SDTDR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    SDTDR->addStatic(Scale(glm::vec3(2.f)));

    // T DS DR
    TransformationComposite* TDSDR = makeComposite();
    TDSDR->addStatic(*t);
    TDSDR->addStatic(Translate(boxesSpread * 40.f));

    TDSDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TDSDR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TDSDR->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // R DT DS
    TransformationComposite* RDTDS = makeComposite();
    RDTDS->addStatic(*t);
    RDTDS->addStatic(Translate(boxesSpread * 41.f));

    RDTDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    RDTDS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    RDTDS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // T DR DS
    TransformationComposite* TDRDS = makeComposite();
    TDRDS->addStatic(*t);
    TDRDS->addStatic(Translate(boxesSpread * 42.f));

    TDRDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TDRDS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TDRDS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // T DR DS
    TransformationComposite* TDRDS_2 = makeComposite();
    TDRDS_2->addStatic(*t);
    TDRDS_2->addStatic(Translate(boxesSpread * 43.f));

    TDRDS_2->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TDRDS_2->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TDRDS_2->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));

    // DS DR DT
    TransformationComposite* DSDRDT = makeComposite();
    DSDRDT->addStatic(*t);
    DSDRDT->addStatic(Translate(boxesSpread * 44.f));

    DSDRDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DSDRDT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DSDRDT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DR DS DT
    TransformationComposite* DRDSDT = makeComposite();
    DRDSDT->addStatic(*t);
    DRDSDT->addStatic(Translate(boxesSpread * 45.f));

    DRDSDT->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DRDSDT->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DRDSDT->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DS DT DR
    TransformationComposite* DSDTDR = makeComposite();
    DSDTDR->addStatic(*t);
    DSDTDR->addStatic(Translate(boxesSpread * 46.f));

    DSDTDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DSDTDR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DSDTDR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // DT DS DR
    TransformationComposite* DTDSDR = makeComposite();
    DTDSDR->addStatic(*t);
    DTDSDR->addStatic(Translate(boxesSpread * 47.f));

    DTDSDR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DTDSDR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DTDSDR->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // DR DT DS
    TransformationComposite* DRDTDS = makeComposite();
    DRDTDS->addStatic(*t);
    DRDTDS->addStatic(Translate(boxesSpread * 48.f));

    DRDTDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DRDTDS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));
    DRDTDS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DT DR DS
    TransformationComposite* DTDRDS = makeComposite();
    DTDRDS->addStatic(*t);
    DTDRDS->addStatic(Translate(boxesSpread * 49.f));

    DTDRDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    DTDRDS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DTDRDS->addDynamic(make<LineMove>(getClock(), glm::vec3(0.f), glm::vec3(-2.f, 0.f, 0.f), 1.f));

    // T(p) DR T(-p)
    TransformationComposite* TpDRTnp = makeComposite();
    TpDRTnp->addStatic(*t);
    TpDRTnp->addStatic(Translate(boxesSpread * 50.f));

    TpDRTnp->addStatic(Translate(glm::vec3(-2.f, 0.f, 0.f)));
    TpDRTnp->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    TpDRTnp->addStatic(Translate(glm::vec3(2.f, 0.f, 0.f)));

    // T(p) DS T(-p)
    TransformationComposite* TpDSTnp = makeComposite();
    TpDSTnp->addStatic(*t);
    TpDSTnp->addStatic(Translate(boxesSpread * 51.f));

    TpDSTnp->addStatic(Translate(glm::vec3(-2.f, 0.f, 0.f)));
    TpDSTnp->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    TpDSTnp->addStatic(Translate(glm::vec3(2.f, 0.f, 0.f)));

    
    // DR T DR S
    TransformationComposite* DRTDRS = makeComposite();
    DRTDRS->addStatic(*t);
    DRTDRS->addStatic(Translate(boxesSpread * 52.f));

    DRTDRS->addStatic(Scale(glm::vec3(2.f)));
    DRTDRS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));
    DRTDRS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRTDRS->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // R DS -R
    TransformationComposite* RDSnegR = makeComposite();
    RDSnegR->addStatic(*t);
    RDSnegR->addStatic(Translate(boxesSpread * 53.f));

    RDSnegR->addStatic(Rotate(-45, glm::vec3(0.f, 1.f, 0.f)));
    RDSnegR->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    RDSnegR->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));

    // DR S -R
    TransformationComposite* DRSnegR = makeComposite();
    DRSnegR->addStatic(*t);
    DRSnegR->addStatic(Translate(boxesSpread * 54.f));

    DRSnegR->addStatic(Rotate(-45, glm::vec3(0.f, 1.f, 0.f)));
    DRSnegR->addStatic(Scale(glm::vec3(2.f)));
    DRSnegR->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    // DS T R S
    TransformationComposite* DSTRS = makeComposite();
    DSTRS->addStatic(*t);
    DSTRS->addStatic(Translate(boxesSpread * 55.f));

    DSTRS->addStatic(Scale(glm::vec3(2.f)));
    DSTRS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    DSTRS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DSTRS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));

    // S T R DS
    TransformationComposite* STRDS = makeComposite();
    STRDS->addStatic(*t);
    STRDS->addStatic(Translate(boxesSpread * 56.f));

    STRDS->addDynamic(make<GrowScale>(getClock(), glm::vec3(0.f), glm::vec3(1.f), 10, glm::vec3(0.0f)));
    STRDS->addStatic(Rotate(45, glm::vec3(0.f, 1.f, 0.f)));
    STRDS->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    STRDS->addStatic(Scale(glm::vec3(2.f)));

    // (DR T) * (DR T S)
    TransformationComposite* DRT_A = makeComposite();
    DRT_A->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRT_A->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    TransformationComposite* DRTS_B = makeComposite();
    DRTS_B->addStatic(Scale(glm::vec3(2.f)));
    DRTS_B->addStatic(Translate(glm::vec3(1.5f, 0.5f, 2.f)));
    DRTS_B->addDynamic(make<DynamicRotate>(getClock(), 45.f, glm::vec3(0.f, 1.f, 0.f)));

    TransformationComposite* DRT_x_DRTS = makeComposite();
    DRT_x_DRTS->addStatic(*t);
    DRT_x_DRTS->addStatic(Translate(boxesSpread * 57.f));

    DRT_x_DRTS->addDynamic(DRT_A);
    DRT_x_DRTS->addDynamic(DRTS_B);

    createObject(resources.getModel("cube"), shader, SRT);
    createObject(resources.getModel("cube"), shader, RST);
    createObject(resources.getModel("cube"), shader, STR);
    createObject(resources.getModel("cube"), shader, TSR);
    createObject(resources.getModel("cube"), shader, RTS);
    createObject(resources.getModel("cube"), shader, TRS);
    createObject(resources.getModel("cube"), shader, DSRT);
    createObject(resources.getModel("cube"), shader, DRST);
    createObject(resources.getModel("cube"), shader, DSTR);
    createObject(resources.getModel("cube"), shader, DTSR);
    createObject(resources.getModel("cube"), shader, DRTS);
    createObject(resources.getModel("cube"), shader, DTRS);
    createObject(resources.getModel("cube"), shader, SDRT);
    createObject(resources.getModel("cube"), shader, RDST);
    createObject(resources.getModel("cube"), shader, SDTR);
    createObject(resources.getModel("cube"), shader, TDSR);
    createObject(resources.getModel("cube"), shader, RDTS);
    createObject(resources.getModel("cube"), shader, TDRS);
    createObject(resources.getModel("cube"), shader, SRDT);
    createObject(resources.getModel("cube"), shader, RSDT);
    createObject(resources.getModel("cube"), shader, STDR);
    createObject(resources.getModel("cube"), shader, TSDR);
    createObject(resources.getModel("cube"), shader, RTDS);
    createObject(resources.getModel("cube"), shader, TRDS);
    createObject(resources.getModel("cube"), shader, DSDRT);
    createObject(resources.getModel("cube"), shader, DRDST);
    createObject(resources.getModel("cube"), shader, DSDTR);
    createObject(resources.getModel("cube"), shader, DTDSR);
    createObject(resources.getModel("cube"), shader, DRDTS);
    createObject(resources.getModel("cube"), shader, DTDRS);
    createObject(resources.getModel("cube"), shader, DSRDT);
    createObject(resources.getModel("cube"), shader, DRSDT);
    createObject(resources.getModel("cube"), shader, DSTDR);
    createObject(resources.getModel("cube"), shader, DTSDR);
    createObject(resources.getModel("cube"), shader, DRTDS);
    createObject(resources.getModel("cube"), shader, DTRDS);
    createObject(resources.getModel("cube"), shader, SDRDT);
    createObject(resources.getModel("cube"), shader, RDSDT);
    createObject(resources.getModel("cube"), shader, SDTDR);
    createObject(resources.getModel("cube"), shader, TDSDR);
    createObject(resources.getModel("cube"), shader, RDTDS);
    createObject(resources.getModel("cube"), shader, TDRDS);
    createObject(resources.getModel("cube"), shader, TDRDS_2);
    createObject(resources.getModel("cube"), shader, DSDRDT);
    createObject(resources.getModel("cube"), shader, DRDSDT);
    createObject(resources.getModel("cube"), shader, DSDTDR);
    createObject(resources.getModel("cube"), shader, DTDSDR);
    createObject(resources.getModel("cube"), shader, DRDTDS);
    createObject(resources.getModel("cube"), shader, DTDRDS);
    createObject(resources.getModel("cube"), shader, TpDRTnp);
    createObject(resources.getModel("cube"), shader, TpDSTnp);
    createObject(resources.getModel("cube"), shader, DRTDRS);
    createObject(resources.getModel("cube"), shader, RDSnegR);
    createObject(resources.getModel("cube"), shader, DRSnegR);
    createObject(resources.getModel("cube"), shader, DSTRS);
    createObject(resources.getModel("cube"), shader, STRDS);
    createObject(resources.getModel("cube"), shader, DRT_x_DRTS);

    buildAxes(location + boxesSpread);
    buildAxes(location + boxesSpread * 2.f);
    buildAxes(location + boxesSpread * 3.f);
    buildAxes(location + boxesSpread * 4.f);
    buildAxes(location + boxesSpread * 5.f);
    buildAxes(location + boxesSpread * 6.f);
    buildAxes(location + boxesSpread * 7.f);
    buildAxes(location + boxesSpread * 8.f);
    buildAxes(location + boxesSpread * 9.f);
    buildAxes(location + boxesSpread * 10.f);
    buildAxes(location + boxesSpread * 11.f);
    buildAxes(location + boxesSpread * 12.f);
    buildAxes(location + boxesSpread * 13.f);
    buildAxes(location + boxesSpread * 14.f);
    buildAxes(location + boxesSpread * 15.f);
    buildAxes(location + boxesSpread * 16.f);
    buildAxes(location + boxesSpread * 17.f);
    buildAxes(location + boxesSpread * 18.f);
    buildAxes(location + boxesSpread * 19.f);
    buildAxes(location + boxesSpread * 20.f);
    buildAxes(location + boxesSpread * 21.f);
    buildAxes(location + boxesSpread * 22.f);
    buildAxes(location + boxesSpread * 23.f);
    buildAxes(location + boxesSpread * 24.f);
    buildAxes(location + boxesSpread * 25.f);
    buildAxes(location + boxesSpread * 26.f);
    buildAxes(location + boxesSpread * 27.f);
    buildAxes(location + boxesSpread * 28.f);
    buildAxes(location + boxesSpread * 29.f);
    buildAxes(location + boxesSpread * 30.f);
    buildAxes(location + boxesSpread * 31.f);
    buildAxes(location + boxesSpread * 32.f);
    buildAxes(location + boxesSpread * 33.f);
    buildAxes(location + boxesSpread * 34.f);
    buildAxes(location + boxesSpread * 35.f);
    buildAxes(location + boxesSpread * 36.f);
    buildAxes(location + boxesSpread * 37.f);
    buildAxes(location + boxesSpread * 38.f);
    buildAxes(location + boxesSpread * 39.f);
    buildAxes(location + boxesSpread * 40.f);
    buildAxes(location + boxesSpread * 41.f);
    buildAxes(location + boxesSpread * 42.f);
    buildAxes(location + boxesSpread * 43.f);
    buildAxes(location + boxesSpread * 44.f);
    buildAxes(location + boxesSpread * 45.f);
    buildAxes(location + boxesSpread * 46.f);
    buildAxes(location + boxesSpread * 47.f);
    buildAxes(location + boxesSpread * 48.f);
    buildAxes(location + boxesSpread * 49.f);
    buildAxes(location + boxesSpread * 50.f);
    buildAxes(location + boxesSpread * 51.f);
    buildAxes(location + boxesSpread * 52.f);
    buildAxes(location + boxesSpread * 53.f);
    buildAxes(location + boxesSpread * 54.f);
    buildAxes(location + boxesSpread * 55.f);
    buildAxes(location + boxesSpread * 56.f);
    buildAxes(location + boxesSpread * 57.f);

}

void TestScene::buildAxes(const glm::vec3& origin) {
    const float length = 3.f;
    const float thickness = 0.03f;

    Material xMaterial;
    xMaterial.ambient = glm::vec3(0.8f, 0.f, 0.f);
    xMaterial.diffuse = glm::vec3(0.4f, 0.f, 0.f);
    xMaterial.specular = glm::vec3(0.f);

    Material yMaterial;
    yMaterial.ambient = glm::vec3(0.f, 0.8f, 0.f);
    yMaterial.diffuse = glm::vec3(0.f, 0.4f, 0.f);
    yMaterial.specular = glm::vec3(0.f);

    Material zMaterial;
    zMaterial.ambient = glm::vec3(0.f, 0.f, 0.8f);
    zMaterial.diffuse = glm::vec3(0.f, 0.f, 0.4f);
    zMaterial.specular = glm::vec3(0.f);

    TransformationComposite* xAxisT = makeComposite();
    xAxisT->addStatic(Translate(origin + glm::vec3(length * 0.5f, 0.f, 0.f)));
    xAxisT->addStatic(Scale(glm::vec3(length, thickness, thickness)));

    TransformationComposite* yAxisT = makeComposite();
    yAxisT->addStatic(Translate(origin + glm::vec3(0.f, length * 0.5f, 0.f)));
    yAxisT->addStatic(Scale(glm::vec3(thickness, length, thickness)));

    TransformationComposite* zAxisT = makeComposite();
    zAxisT->addStatic(Translate(origin + glm::vec3(0.f, 0.f, length * 0.5f)));
    zAxisT->addStatic(Scale(glm::vec3(thickness, thickness, length)));

    createObject(resources.getModel("cube"), shader, xAxisT)->setMaterial(xMaterial);
    createObject(resources.getModel("cube"), shader, yAxisT)->setMaterial(yMaterial);
    createObject(resources.getModel("cube"), shader, zAxisT)->setMaterial(zMaterial);
}