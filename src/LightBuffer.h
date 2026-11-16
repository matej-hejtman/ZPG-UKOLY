// Author: Matěj Hejtman, login: HEJ0094
#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>

constexpr int MAX_LIGHTS = 8; // Musi sedet ve vsech shaderech!
constexpr GLuint LIGHT_BINDING_POINT = 0;

struct LightData {
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 attenuation;
	glm::vec4 direction;
	glm::vec4 params;		// x = type, y = cutOff, z = outerCutOff, w = zatim nic
};

static_assert(sizeof(LightData) % 16 == 0, "LightData musi byt nasobek 16B");

class LightBuffer {
public:
	LightBuffer();
	~LightBuffer();
	LightBuffer(const LightBuffer&) = delete;
	LightBuffer& operator=(const LightBuffer&) = delete;

	void update(const std::vector<LightData>& data) const;

private:
	GLuint ubo = 0;
};