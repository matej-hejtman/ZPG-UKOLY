// Author: Matt Hejtman, login: HEJ0094
#include "Application.h"

int main() {
    Application app;
    app.initialization();
    app.createShaders();
    app.createModels();
    app.run();
    return 0;
}
