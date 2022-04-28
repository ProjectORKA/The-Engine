
#include "VertexBufferObject.hpp"

void VertexBufferObject::create(UInt location, Float* data, UInt byteSize, UInt usage, Index components)
{
	if (!loaded) {

		this->location = location;
		this->components = components;
		this->byteSize = byteSize;
		this->usage = usage;
		apiGenBuffer(bufferID);
		apiBindBuffer(GL_ARRAY_BUFFER, bufferID);
		apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		loaded = true;
	}
	else {
		logError("VBO alredy loaded");
	}
}

void VertexBufferObject::update(Float* data, UInt byteSize)
{
	if (loaded) {
		this->byteSize = byteSize;
		apiBindBuffer(GL_ARRAY_BUFFER, bufferID);
		apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		loaded = true;
	}
}

void VertexBufferObject::destroy()
{
	if (loaded) {
		apiBindBuffer(GL_ARRAY_BUFFER, 0);
		apiDeleteBuffer(bufferID);
		loaded = false;
	}
}

void VertexBufferObject::attach() {
	apiEnableVertexAttribArray(location);
	apiVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void VertexBufferObject::detach() {
	apiDisableVertexAttribArray(location);
}
