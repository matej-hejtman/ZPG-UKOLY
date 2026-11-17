// Author: Matěj Hejtman, login: HEJ0094
#pragma once

// Cas sceny pro animovane transformace (DynamicRotate, Oscillate, GrowScale, LineMove).
// Posouva ho scena jednou za snimek (Scene::tick), takze:
//   - vsechny getMatrix() v jednom snimku vidi stejny cas,
//   - jde pozastavit (setPaused) a zrychlit / zpomalit (setSpeed),
//   - kazda scena ma vlastni cas a animace zacinaji, az kdyz je scena aktivni.
class SceneClock {
public:
    void advance(float realDeltaTime);

    double getTime() const;       // cas sceny v sekundach (od vytvoreni sceny, bez pauz)
    float getDelta() const;       // o kolik se cas posunul v poslednim snimku (po pauze / rychlosti)

    void setPaused(bool value);
    bool isPaused() const;

    void setSpeed(float value);   // 1 = normalne, 0.5 = zpomalene, 2 = zrychlene
    float getSpeed() const;

private:
    double time = 0.0;
    float delta = 0.0f;
    float speed = 1.0f;
    bool paused = false;
};
