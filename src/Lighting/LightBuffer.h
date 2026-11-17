// Author: Matěj Hejtman, login: HEJ0094
#pragma once
#include <glad/gl.h>
#include <glm/glm.hpp>

#include <vector>

constexpr int MAX_LIGHTS = 64; // Musi sedet ve vsech shaderech!
constexpr GLuint LIGHT_BINDING_POINT = 0;

struct LightData {
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 attenuation;
	glm::vec4 direction;
	glm::vec4 params;
};

struct LightBufferHeader {
	glm::ivec4 info;
	glm::vec4 ambientLight;
};

static_assert(sizeof(LightData) % 16 == 0, "LightData musi byt nasobek 16B");
static_assert(sizeof(LightBufferHeader) == 32, "LightBufferHeader musi mit 32B (std140)");

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
