// Author: Matěj Hejtman, login: HEJ0094
#pragma once

class Application;

// Globalni klavesy, funguji v kazde scene:
//   Esc ... konec, Tab / Shift+Tab ... dalsi / predchozi scena, F11 ... fullscreen
// Dostane klavesu jako prvni; kdyz ji obslouzi, scena ji uz nedostane.
class MainInput {
public:
    explicit MainInput(Application& app);

    bool onKey(int key, int action, int mods);

private:
    Application& app;
};
