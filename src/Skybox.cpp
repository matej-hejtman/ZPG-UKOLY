// Author: Matt Hejtman, login: HEJ0094
#include "Skybox.h"
#include "CubeMap.h"
#include "Model.h"
#include "ShaderProgram.h"

Skybox::Skybox(Model* cube, ShaderProgram* program, CubeMap* cubeMap) : cube(cube), program(program), cubeMap(cubeMap) {}

void Skybox::draw() const {
    glDepthMask(GL_FALSE);
    program->use();
    cubeMap->bind(0);
    program->setUniform("skybox", 0);
    cube->draw();
    glDepthMask(GL_TRUE);
}
