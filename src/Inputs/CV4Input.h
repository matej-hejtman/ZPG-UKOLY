// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include "Inputs/SceneInput.h"

class CV4Scene;

class CV4Input : public SceneInput {
public:
    explicit CV4Input(CV4Scene& scene);

    void onKey(GLFWwindow* window, int key, int action, int mods) override;

private:
    CV4Scene& cv4Scene;
	bool ringsVisible = true;
};
