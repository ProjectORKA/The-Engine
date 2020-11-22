
#include "IndexBufferObject.hpp"

void IndexBufferObject::create(Index* data, UInt indexCount, UInt usage)
{
	this->indexCount = indexCount;

	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}
void IndexBufferObject::unload()
{
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);
}