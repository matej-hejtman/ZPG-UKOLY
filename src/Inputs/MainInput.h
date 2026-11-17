// Author: Matěj Hejtman, login: HEJ0094
#pragma once

class Application;

class MainInput {
public:
    explicit MainInput(Application& app);

    bool onKey(int key, int action, int mods);

private:
    Application& app;
};
