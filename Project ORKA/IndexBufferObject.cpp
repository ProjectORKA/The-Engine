#include "IndexBufferObject.hpp"

void IndexBufferObject::create(const Index* data, const UInt indexCount, const UInt usage) {
	this->indexCount = indexCount;

	apiGenBuffer(bufferID);
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}

void IndexBufferObject::update(const Index* data, const UInt indexCount, const UInt usage) {
	this->indexCount = indexCount;
	apiBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	apiBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), data, usage);
}

void IndexBufferObject::unload() const { apiDeleteBuffer(bufferID); }
