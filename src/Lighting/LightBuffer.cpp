#include "LightBuffer.h"

#include <algorithm>

LightBuffer::LightBuffer() {
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData) * MAX_LIGHTS, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_BINDING_POINT, ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

LightBuffer::~LightBuffer() {
	if (ubo != 0) {
		glDeleteBuffers(1, &ubo);
	}
}

void LightBuffer::update(const std::vector<LightData>& data) const {
	const std::size_t count = std::min(data.size(), static_cast<std::size_t>(MAX_LIGHTS));
	if (count == 0) {
		return;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData) * count, data.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}