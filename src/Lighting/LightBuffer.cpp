// Author: Matěj Hejtman, login: HEJ0094
#include "Lighting/LightBuffer.h"

#include <algorithm>

LightBuffer::LightBuffer() {
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightBufferHeader) + sizeof(LightData) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightBuffer::~LightBuffer() {
	if (ubo != 0) {
		glDeleteBuffers(1, &ubo);
	}
}

void LightBuffer::update(const std::vector<LightData>& lights, const glm::vec3& ambientLight) const {
	const std::size_t count = std::min(lights.size(), static_cast<std::size_t>(MAX_LIGHTS));

	LightBufferHeader header;
	header.info = glm::ivec4(static_cast<int>(count), 0, 0, 0);
	header.ambientLight = glm::vec4(ambientLight, 1.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightBufferHeader), &header);
	
	if (count > 0) {
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(LightBufferHeader), sizeof(LightData) * count, lights.data());
	}
	
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
