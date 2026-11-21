// Author: Matěj Hejtman, login: HEJ0094
#include "Scenes/CV4Scene.h"
#include "Core/Resources.h"
#include "Rendering/DrawableObject.h"
#include "Lighting/Material.h"
#include "Transformations\DynamicRotate.h"

namespace {
	const char* PALETTE = "assets/models/space_models/space_palette.png";

	Material planetMaterial(Resources& resources) {
		return Material({
			glm::vec3(0.20f),
			glm::vec3(0.90f),
			glm::vec3(0.15f),
			16.0f,
			resources.getTexture(PALETTE)
		});
	}
}

void CV4Scene::create() {

	shader = resources.getShader("lit.vert", "phong.frag");
	sunShader = resources.getShader("basic.vert", "basic.frag");

	removeGround();
	setAmbientLight(glm::vec3(0.0f));

	Skybox* sb = resources.getSkybox("assets/textures/Skyboxes/skybox_forest_dawn_night");
	setSkybox(sb);

	getCamera()->setPosition(glm::vec3(0.f, 18.f, 30.f), 121.f, -90.f);

	buildSun();
	buildMecrury();
	buildVenus();
	buildEarth();
	buildMars();
	buildJupiter();
	buildSaturn();
	buildUran();
	buildNeptune();
	buildPluto();
	buildProps();
	buildOrbits();
}

void CV4Scene::update(float deltaTime) {
    (void)deltaTime;
}


void CV4Scene::buildSun() {
	TransformationComposite* sunT = makeComposite();

	sunT->addDynamic(make<DynamicRotate>(getClock(), 15.f, glm::vec3(1.f, 1.f, 1.f), 0.f));
	auto sun = createObject(resources.getModel("sun"), sunShader, sunT);

	sun->setTexture(resources.getTexture(PALETTE));

	Light* sunLight = addLight();
	sunLight->setType(LightType::Point);
	sunLight->setPosition(glm::vec3(0.f, 0.f, 0.f));
	sunLight->setColor(glm::vec3(1.f, 0.7f, 0.5f));
	sunLight->setIntensity(5.0f);
	sunLight->setAttenuation(2.5f, 0.05f, 0.00075f);
}

TransformationComposite* CV4Scene::createPlanetFrame(float distance, float orbitSpeed, float startAngle, float axialTilt, float inclination) {
	TransformationComposite* frame = makeComposite();

	if (inclination != 0.f) {
		frame->addStatic(Rotate(inclination, glm::vec3(0.f, 0.f, 1.f)));
	}

	frame->addDynamic(make<DynamicRotate>(getClock(), orbitSpeed, glm::vec3(0.f, 1.f, 0.f), startAngle));
	frame->addStatic(Translate(glm::vec3(distance, 0.f, 0.f)));

	frame->addDynamic(make<DynamicRotate>(getClock(), -orbitSpeed, glm::vec3(0.f, 1.f, 0.f), -startAngle));

	if (axialTilt != 0.f) {
		frame->addStatic(Rotate(axialTilt, glm::vec3(0.f, 0.f, 1.f)));
	}

	return frame;
}

DrawableObject* CV4Scene::createPlanet(const char* model, TransformationComposite* frame, float spinSpeed, float scale) {
	TransformationComposite* planetT = make<TransformationComposite>();
	planetT->addDynamic(frame);
	planetT->addDynamic(make<DynamicRotate>(getClock(), spinSpeed, glm::vec3(0.f, 1.f, 0.f), 0.f));
	planetT->addStatic(Scale(glm::vec3(scale)));

	auto planet = createObject(resources.getModel(model), shader, planetT);
	planet->setMaterial(planetMaterial(resources));
	return planet;
}

DrawableObject* CV4Scene::createMoon(const char* model, TransformationComposite* frame, float distance, float orbitSpeed, float startAngle, float scale) {
	TransformationComposite* moonT = make<TransformationComposite>();
	moonT->addDynamic(frame);
	moonT->addDynamic(make<DynamicRotate>(getClock(), orbitSpeed, glm::vec3(0.f, 1.f, 0.f), startAngle));
	moonT->addStatic(Translate(glm::vec3(distance, 0.f, 0.f)));
	moonT->addStatic(Scale(glm::vec3(scale)));

	auto moon = createObject(resources.getModel(model), shader, moonT);
	moon->setMaterial(planetMaterial(resources));
	return moon;
}

DrawableObject* CV4Scene::createOrbit(float distance, float inclination) {
	TransformationComposite* ringT = makeComposite();

	if (inclination != 0.f) {
		ringT->addStatic(Rotate(inclination, glm::vec3(0.f, 0.f, 1.f)));
	}

	ringT->addStatic(Scale(glm::vec3(distance, 1.f, distance)));

	auto ring = createObject(resources.getModel("orbit_ring"), sunShader, ringT);
	ring->setTexture(resources.getTexture(PALETTE));
	return ring;
}

void CV4Scene::buildMecrury() {
	TransformationComposite* frame = createPlanetFrame(6.5f, 12.2f, 0.f, 0.f);
	createPlanet("planet_mercury", frame, 10.f, 0.111f);
}

void CV4Scene::buildVenus() {
	TransformationComposite* frame = createPlanetFrame(8.6f, 7.7f, 67.f, 177.4f);
	createPlanet("planet_venus", frame, 4.f, 0.175f);
}

void CV4Scene::buildEarth() {
	TransformationComposite* frame = createPlanetFrame(10.f, 6.f, 140.f, 23.4f);
	createPlanet("planet_earth", frame, 45.f, 0.18f);

	createMoon("moon_luna", frame, 0.6f, 40.f, 0.f, 0.094f);
}

void CV4Scene::buildMars() {
	TransformationComposite* frame = createPlanetFrame(12.3f, 4.4f, 210.f, 25.2f);
	createPlanet("planet_mars", frame, 44.f, 0.131f);

	createMoon("moon_phobos", frame, 0.24f, 90.f, 0.f, 0.035f);
	createMoon("moon_deimos", frame, 0.36f, 45.f, 120.f, 0.025f);
}

void CV4Scene::buildJupiter() {
	TransformationComposite* frame = createPlanetFrame(23.5f, 1.74f, 300.f, 3.1f);
	createPlanet("planet_jupiter", frame, 110.f, 0.6f);

	createMoon("moon_io", frame, 1.0f, 60.f, 0.f, 0.096f);
	createMoon("moon_europa", frame, 1.3f, 40.f, 110.f, 0.089f);
	createMoon("moon_ganymede", frame, 1.7f, 25.f, 230.f, 0.116f);
}

void CV4Scene::buildSaturn() {
	TransformationComposite* frame = createPlanetFrame(33.f, 1.1f, 30.f, 26.7f);
	createPlanet("planet_saturn", frame, 100.f, 1.33f);

	createMoon("moon_titan", frame, 2.0f, 20.f, 0.f, 0.114f);
}

void CV4Scene::buildUran() {
	TransformationComposite* frame = createPlanetFrame(49.f, 0.65f, 120.f, 97.8f);
	createPlanet("planet_uranus", frame, 64.f, 0.557f);
}

void CV4Scene::buildNeptune() {
	TransformationComposite* frame = createPlanetFrame(63.6f, 0.47f, 250.f, 28.3f);
	createPlanet("planet_neptune", frame, 67.f, 0.354f);
}

void CV4Scene::buildPluto() {
	TransformationComposite* frame = createPlanetFrame(74.6f, 0.38f, 330.f, 122.5f, plutoInclination);
	createPlanet("planet_pluto", frame, 7.f, 0.078f);
}

void CV4Scene::buildProps() {
	const glm::vec3 Y(0.f, 1.f, 0.f);

	auto prop = [&](const char* model, TransformationComposite* t, bool lit) {
		DrawableObject* object = createObject(resources.getModel(model), lit ? shader : sunShader, t);
		if (lit) {
			object->setMaterial(planetMaterial(resources));
		} else {
			object->setTexture(resources.getTexture(PALETTE));
		}
		return object;
	};

	auto orbitFrame = [&](float distance, float speed, float startAngle, float height = 0.f, float inclination = 0.f) {
		TransformationComposite* t = makeComposite();
		if (inclination != 0.f) {
			t->addStatic(Rotate(inclination, glm::vec3(0.f, 0.f, 1.f)));
		}
		t->addDynamic(make<DynamicRotate>(getClock(), speed, Y, startAngle));
		t->addStatic(Translate(glm::vec3(distance, height, 0.f)));
		return t;
	};

	auto child = [&](TransformationComposite* frame) {
		TransformationComposite* t = make<TransformationComposite>();
		t->addDynamic(frame);
		return t;
	};

	const char* asteroids[] = { "asteroid_1", "asteroid_2", "asteroid_3", "asteroid_4" };
	for (int i = 0; i < 16; i++) {

		const float distance = 16.f + static_cast<float>((i * 7) % 10) * 0.4f;
		const float height = static_cast<float>((i * 5) % 7 - 3) * 0.15f;
		const float speed = 2.9f - static_cast<float>(i % 4) * 0.15f;

		TransformationComposite* t = orbitFrame(distance, speed, static_cast<float>(i) * 137.5f, height);
		t->addDynamic(make<DynamicRotate>(getClock(), 20.f + static_cast<float>(i) * 3.f, glm::normalize(glm::vec3(1.f, static_cast<float>(i % 3), 1.f)), 0.f));
		t->addStatic(Scale(glm::vec3(0.08f + static_cast<float>(i % 3) * 0.04f)));
		prop(asteroids[i % 4], t, true);
	}

	for (int i = 0; i < 3; i++) {
		TransformationComposite* t = orbitFrame(17.5f + static_cast<float>(i) * 1.1f, 2.7f, 40.f + static_cast<float>(i) * 120.f, 0.2f);
		t->addDynamic(make<DynamicRotate>(getClock(), 12.f, glm::normalize(glm::vec3(0.3f, 1.f, 0.2f)), 0.f));
		t->addStatic(Scale(glm::vec3(0.25f)));
		prop("asteroid_cluster", t, true);
	}

	for (int i = 0; i < 3; i++) {
		TransformationComposite* t = orbitFrame(18.5f - static_cast<float>(i) * 1.3f, 2.8f, 95.f + static_cast<float>(i) * 120.f, -0.2f);
		t->addDynamic(make<DynamicRotate>(getClock(), 30.f, glm::normalize(glm::vec3(1.f, 0.5f, 0.f)), 0.f));
		t->addStatic(Scale(glm::vec3(0.15f)));
		prop("asteroid_crystal", t, true);
	}

	TransformationComposite* t = orbitFrame(40.f, 0.9f, 200.f, 0.f, 35.f);
	t->addStatic(Rotate(180.f, Y));
	t->addStatic(Scale(glm::vec3(0.6f)));
	prop("comet", t, false);

	t = orbitFrame(14.f, 5.f, 30.f, 0.f, 10.f);
	t->addStatic(Rotate(-90.f, Y));
	t->addStatic(Scale(glm::vec3(0.15f)));
	prop("meteor", t, true);

	t = orbitFrame(27.f, 2.f, 260.f, 0.f, -8.f);
	t->addStatic(Rotate(-90.f, Y));
	t->addStatic(Scale(glm::vec3(0.2f)));
	prop("meteor", t, true);

	TransformationComposite* station = orbitFrame(10.6f, 6.f, 128.f, -0.3f);
	t = child(station);
	t->addDynamic(make<DynamicRotate>(getClock(), 10.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(0.2f)));
	prop("space_station", t, true);

	t = child(station);
	t->addStatic(Translate(glm::vec3(0.35f, 0.1f, 0.f)));
	t->addDynamic(make<DynamicRotate>(getClock(), 15.f, glm::normalize(glm::vec3(1.f, 0.f, 1.f)), 0.f));
	t->addStatic(Scale(glm::vec3(0.04f)));
	prop("astronaut", t, true);

	t = orbitFrame(10.4f, 6.f, 150.f, 0.3f);
	t->addDynamic(make<DynamicRotate>(getClock(), 20.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(0.07f)));
	prop("satellite", t, true);

	t = orbitFrame(12.7f, 4.4f, 215.f, 0.15f);
	t->addDynamic(make<DynamicRotate>(getClock(), 20.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(0.07f)));
	prop("satellite", t, true);

	t = orbitFrame(9.4f, 7.f, 120.f, 0.f, 5.f);
	t->addStatic(Rotate(-90.f, glm::vec3(1.f, 0.f, 0.f)));
	t->addStatic(Scale(glm::vec3(0.08f)));
	prop("rocket", t, true);

	TransformationComposite* ufo = orbitFrame(10.9f, 6.f, 160.f, 0.8f);
	t = child(ufo);
	t->addDynamic(make<DynamicRotate>(getClock(), 90.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(0.12f)));
	prop("ufo", t, true);

	t = child(ufo);
	t->addStatic(Translate(glm::vec3(0.f, -0.3f, 0.f)));
	t->addDynamic(make<DynamicRotate>(getClock(), -30.f, Y - 1.f, 0.f));
	t->addStatic(Scale(glm::vec3(0.06f)));
	prop("alien", t, true);

	t = orbitFrame(88.f, 0.3f, 60.f, 0.f, 12.f);
	t->addDynamic(make<DynamicRotate>(getClock(), 20.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(0.45f)));
	prop("planet_alien", t, true);

	TransformationComposite* rock = orbitFrame(82.f, 0.33f, 170.f, 0.f, -6.f);
	t = child(rock);
	t->addStatic(Scale(glm::vec3(0.6f)));
	prop("asteroid_2", t, true);

	t = child(rock);
	t->addStatic(Translate(glm::vec3(0.f, 0.72f, 0.f)));
	t->addStatic(Scale(glm::vec3(0.25f)));
	prop("monolith", t, true);

	t = child(rock);
	t->addStatic(Translate(glm::vec3(0.5f, 0.72f, 0.f)));
	t->addStatic(Scale(glm::vec3(0.25f)));
	prop("login", t, true);


	t = child(rock);
	t->addStatic(Translate(glm::vec3(0.3f, 0.55f, 0.2f)));
	t->addStatic(Rotate(-30.f, Y));
	t->addStatic(Scale(glm::vec3(0.07f)));
	prop("moon_rover", t, true);

	t = makeComposite();
	t->addStatic(Translate(glm::vec3(-95.f, 12.f, -55.f)));
	t->addStatic(Rotate(20.f, glm::vec3(1.f, 0.f, 0.f)));
	t->addDynamic(make<DynamicRotate>(getClock(), 25.f, Y, 0.f));
	t->addStatic(Scale(glm::vec3(4.f)));
	prop("black_hole", t, false);
}

void CV4Scene::buildOrbits() {
	orbits.push_back(createOrbit(6.5f));
	orbits.push_back(createOrbit(8.6f));
	orbits.push_back(createOrbit(10.f));
	orbits.push_back(createOrbit(12.3f));
	orbits.push_back(createOrbit(23.5f));
	orbits.push_back(createOrbit(33.f));
	orbits.push_back(createOrbit(49.f));
	orbits.push_back(createOrbit(63.6f));
	orbits.push_back(createOrbit(74.6f, plutoInclination));
}

void CV4Scene::setRingsVisible(bool visible) {
	for (DrawableObject* orbit : orbits) {
		orbit->setVisible(visible);
	}
}
