
#include "VertexBufferObject.hpp"

void VertexBufferObject::create(UInt location, float* data, UInt byteSize, UInt usage, Index components)
{
	if (!loaded) {

		this->location = location;
		this->components = components;
		this->byteSize = byteSize;
		glGenBuffers(1, &bufferID);
		glBindBuffer(GL_ARRAY_BUFFER, bufferID);
		glBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, (void*)0);
		loaded = true;
	}

}

void VertexBufferObject::unload()
{
	if (loaded) {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &bufferID);
		loaded = false;
	}
}
