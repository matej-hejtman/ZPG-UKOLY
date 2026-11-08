// Author: Matt Hejtman, login: HEJ0094
#pragma once

class Model;
class ShaderProgram;
class CubeMap;

class Skybox {
public:
    Skybox(Model* cube, ShaderProgram* program, CubeMap* cubeMap);
    void draw() const;

private:
    Model* cube;
    ShaderProgram* program;
    CubeMap* cubeMap;
};
