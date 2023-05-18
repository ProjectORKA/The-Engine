#include "GPUBuffer.hpp"
#include "GraphicsAPI.hpp"

void GPUBuffer::create(const Vector<Vec4>& data, const Int location) {
	if (!loaded) {
		glGenBuffers(1, &bufferId);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vec4) * data.size(), data.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, bufferId);
		loaded = true;
	}
}

void GPUBuffer::use(const Int location) const {
	if (loaded) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferId);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, bufferId);
	}
	else logError("SSBO not loaded!");
}
