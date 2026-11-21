// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Core/Scene.h"

class CV4Scene : public Scene {
public:
    using Scene::Scene;

    void create() override;
    void update(float deltaTime) override;

	void setRingsVisible(bool visible);

private:

    void buildSun();
    void buildMecrury();
    void buildVenus();
    void buildEarth();
    void buildMars();
    void buildJupiter();
    void buildSaturn();
    void buildUran();
	void buildNeptune();
	void buildPluto();
    void buildProps();
    void buildOrbits();


    TransformationComposite* createPlanetFrame(float distance, float orbitSpeed, float startAngle, float axialTilt, float inclination = 0.f);
    DrawableObject* createPlanet(const char* model, TransformationComposite* frame, float spinSpeed, float scale);
    DrawableObject* createMoon(const char* model, TransformationComposite* frame, float distance, float orbitSpeed, float startAngle, float scale);
    DrawableObject* createOrbit(float distance, float inclination = 0.f);

    ShaderProgram* shader = nullptr;
    ShaderProgram* sunShader = nullptr;

	Light* sunlight;

    const float plutoInclination = 17.f;

    std::vector<DrawableObject*> orbits;
};
