// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

constexpr int MAX_LIGHTS = 64; // Musi sedet ve vsech shaderech!
constexpr GLuint LIGHT_BINDING_POINT = 0;

// Jedno svetlo v UBO (std140, 5x vec4 = 80 B). Plni ho Light::toData().
struct LightData {
	glm::vec4 position;     // xyz = pozice ve svete
	glm::vec4 color;        // rgb = barva
	glm::vec4 attenuation;  // x = constant, y = linear, z = quadratic
	glm::vec4 direction;    // xyz = normalizovany smer ve svete
	glm::vec4 params;       // x = typ (0 point, 1 directional, 2 spot), y = cos(vnitrni uhel), z = cos(vnejsi uhel)
};

// Hlavicka UBO pred polem svetel (std140, 2x 16 B):
//   ivec4 info;          x = pocet svetel
//   vec4  ambientLight;  rgb = ambientni svetlo sceny
struct LightBufferHeader {
	glm::ivec4 info;
	glm::vec4 ambientLight;
};

static_assert(sizeof(LightData) % 16 == 0, "LightData musi byt nasobek 16B");
static_assert(sizeof(LightBufferHeader) == 32, "LightBufferHeader musi mit 32B (std140)");

// Uniform buffer se svetly sdileny vsemi shadery (blok "LightBuffer").
// Pocet svetel i ambient jsou primo v bufferu -> jediny zdroj pravdy, zadne uniformy navic.
class LightBuffer {
public:
	LightBuffer();
	~LightBuffer();
	LightBuffer(const LightBuffer&) = delete;
	LightBuffer& operator=(const LightBuffer&) = delete;

	void update(const std::vector<LightData>& lights, const glm::vec3& ambientLight) const;

private:
	GLuint ubo = 0;
};
