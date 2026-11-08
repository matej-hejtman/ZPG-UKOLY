// Author: Matt Hejtman, login: HEJ0094
#pragma once

class Camera;

class ICameraObserver {
public:
    virtual ~ICameraObserver() = default;
    virtual void onCameraChanged(Camera* camera) = 0;
};
