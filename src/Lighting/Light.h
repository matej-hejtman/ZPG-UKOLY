// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Lighting/LightBuffer.h"
#include "Transformations/TransformationComposite.h"

#include <glm/glm.hpp>

enum class LightType {
    Point = 0,
    Directional = 1,
    Spot = 2
};

// Svetlo s vlastni transformaci. Pozice a smer jsou v lokalnich souradnicich svetla,
// do sveta je prevede getTransformation() - stejny system jako u objektu, takze svetlo
// jde pripnout k cemukoliv (obihani, LineMove, nasledovani objektu...).
// Scene::addLight() transformaci napoji na base sceny (jako makeComposite()).
class Light {
public:
    void setPosition(const glm::vec3& value);
    const glm::vec3& getPosition() const;

    void setColor(const glm::vec3& value);
    const glm::vec3& getColor() const;

    void setAttenuation(float constantValue, float linearValue, float quadraticValue);
    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;

    void setEnabled(bool value);
    bool isEnabled() const;

    void setType(LightType value);
    LightType getType() const;

    // smer se normalizuje (smerove svetlo a reflektor)
    void setDirection(const glm::vec3& value);
    const glm::vec3& getDirection() const;

    // kuzel reflektoru ve STUPNICH (polovicni uhel od osy), vnejsi musi byt >= vnitrni.
    // Mezi vnitrnim a vnejsim uhlem svetlo plynule slabne.
    void setSpotAngles(float innerDeg, float outerDeg);
    void setCutOff(float innerDeg);
    float getCutOff() const;
    void setOuterCutOff(float outerDeg);
    float getOuterCutOff() const;

    // transformace svetla (napojena na base sceny, pokud ho vytvoril Scene::addLight)
    TransformationComposite& getTransformation();

    // data pro GPU: pozice a smer ve svete (smer znovu normalizovany, protoze
    // transformace muze obsahovat meritko - napr. zoom sceny)
    LightData toData() const;

private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(1.0f);
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    bool enabled = true;

    LightType type = LightType::Point;
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float cutOffDeg = 12.5f;
	float outerCutOffDeg = 17.5f;

    TransformationComposite transformation;
};
