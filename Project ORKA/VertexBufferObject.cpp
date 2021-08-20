
#include "VertexBufferObject.hpp"

void VertexBufferObject::create(UInt location, float* data, UInt byteSize, UInt usage, Index components)
{
	if (!loaded) {

		this->location = location;
		this->components = components;
		this->byteSize = byteSize;
		apiGenBuffer(bufferID);
		apiBindBuffer(GL_ARRAY_BUFFER, bufferID);
		apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		apiEnableVertexAttribArray(location);
		apiVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, (void*)0);
		loaded = true;
	}
	else {
		logError("VBO alredy loaded");
	}
}

//void VertexBufferObject::update(UInt location, float* data, UInt byteSize, UInt usage, Index components)
//{
//	this->location = location;
//	this->components = components;
//	this->byteSize = byteSize;
//	apiBindBuffer(GL_ARRAY_BUFFER, bufferID);
//	apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
//	apiEnableVertexAttribArray(location);
//	apiVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, (void*)0);
//	loaded = true;
//}

void VertexBufferObject::unload()
{
	if (loaded) {
		apiBindBuffer(GL_ARRAY_BUFFER, 0);
		apiDeleteBuffer(bufferID);
		loaded = false;
	}
}
