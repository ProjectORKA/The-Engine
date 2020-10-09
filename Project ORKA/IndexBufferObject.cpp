
#include "IndexBufferObject.hpp"

void IndexBufferObject::create(Index* data, UInt indexCount, UInt usage)
{
	this->indexCount = indexCount;

#ifdef GRAPHICS_API_OPENGL
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
#endif // GRAPHICS_API_OPENGL
}
void IndexBufferObject::unload()
{
#ifdef GRAPHICS_API_OPENGL
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);
#endif // GRAPHICS_API_OPENGL
}