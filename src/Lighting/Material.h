// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class ShaderProgram;
class Texture;

struct Material {
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(0.5f);
    float shininess = 32.0f;
    Texture* texture = nullptr;

    void apply(ShaderProgram* program) const;

    // ------ METALS ------
    static Material gold();
    static Material silver();
    static Material copper();
    static Material bronze();
    static Material brass();
    static Material chrome();
    static Material pewter();

    // ------ POLISHED METALS ------
    static Material polishedGold();
    static Material polishedSilver();
    static Material polishedCopper();
    static Material polishedBronze();

    // ------ GEMS & STONES ------
    static Material emerald();
    static Material ruby();
    static Material jade();
    static Material obsidian();
    static Material pearl();
    static Material turquoise();

    // ------ PLASTICS ------
    static Material blackPlastic();
    static Material whitePlastic();
    static Material redPlastic();
    static Material greenPlastic();
    static Material cyanPlastic();
    static Material yellowPlastic();

    // ------ RUBBERS ------
    static Material blackRubber();
    static Material whiteRubber();
    static Material redRubber();
    static Material greenRubber();
    static Material cyanRubber();
    static Material yellowRubber();
};
