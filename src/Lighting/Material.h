// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glm/glm.hpp>

class ShaderProgram;
class Texture;

// Jak povrch reaguje na svetlo. Koeficienty se nasobi zakladni barvou objektu
// (objectColor * textura / barva vrcholu).
// Kazdy DrawableObject ma vlastni KOPII materialu: zmena u jednoho objektu
// (napr. zvyrazneni) neovlivni ostatni, ale "zmen zlatou vsem" udelat nejde.
struct Material {
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(0.5f);
    float shininess = 32.0f;            // min. 1 (pow(x, 0) = 1 by dal odlesk pres cely povrch)
    Texture* texture = nullptr;         // difuzni textura, nullptr = barva objektu / vrcholu

    // aktivni program (use() uz probehlo) dostane material.* a texturu
    void apply(ShaderProgram* program) const;

    // klasicka tabulka materialu (barva je v koeficientech -> objekt nechat bily)
    static Material gold();
    static Material silver();
    static Material copper();
    static Material bronze();
    static Material chrome();
    static Material pearl();
    static Material emerald();
    static Material ruby();
    static Material plastic();          // bily matny plast s ostrym odleskem
    static Material rubber();           // bila guma, siroky matny odlesk
};
