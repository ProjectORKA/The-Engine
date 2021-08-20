
#include "IndexBufferObject.hpp"

void IndexBufferObject::create(Index* data, UInt indexCount, UInt usage)
{
	this->indexCount = indexCount;

	apiGenBuffer(bufferID);
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}
void IndexBufferObject::update(Index* data, UInt indexCount, UInt usage)
{
	this->indexCount = indexCount;
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}
void IndexBufferObject::unload()
{
	apiDeleteBuffer(bufferID);
}