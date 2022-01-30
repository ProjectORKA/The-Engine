#include "GPUBuffer.hpp"
#include "GraphicsAPI.hpp"

void GPUBuffer::create(Vector<Vec4>& data, Int location) {
	if (!loaded) {
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Vec4) * data.size(), data.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER,location,bufferID);
		loaded = true;
	}
}

void GPUBuffer::use(Int location) {
	if (loaded) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, bufferID);
	}
	else logError("SSBO not loaded!");
}