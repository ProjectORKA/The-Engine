#include "IndexBufferObject.hpp"

void IndexBufferObject::create(const Index* data, const ULL indexCount, const MeshDrawMode usage)
{
	this->indexCount = indexCount;

	apiGenBuffer(bufferId);
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}

void IndexBufferObject::update(const Index* data, const ULL indexCount, const MeshDrawMode usage)
{
	this->indexCount = indexCount;
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}

void IndexBufferObject::unload() const
{
	apiDeleteBuffer(bufferId);
}