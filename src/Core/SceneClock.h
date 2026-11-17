// Author: Matěj Hejtman, login: HEJ0094
#pragma once

class SceneClock {
public:
    void advance(float realDeltaTime);

    double getTime() const;
    float getDelta() const;

    void setPaused(bool value);
    bool isPaused() const;

    void setSpeed(float value);
    float getSpeed() const;

private:
    double time = 0.0;
    float delta = 0.0f;
    float speed = 1.0f;
    bool paused = false;
};
