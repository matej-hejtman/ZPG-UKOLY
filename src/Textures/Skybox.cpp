// Author: Matěj Hejtman, login: HEJ0094
#include "Textures/Skybox.h"
#include "Textures/CubeMap.h"
#include "Rendering/Model.h"
#include "Rendering/ShaderProgram.h"

Skybox::Skybox(Model* cube, ShaderProgram* program, CubeMap* cubeMap) : cube(cube), program(program), cubeMap(cubeMap) {}

void Skybox::draw() const {
    glDepthMask(GL_FALSE);
    program->use();
    cubeMap->bind(0);
    program->setUniform("skybox", 0);
    cube->draw();
    glDepthMask(GL_TRUE);
}

ShaderProgram* Skybox::getProgram() const {
    return program;
}
