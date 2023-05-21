#include "VertexBufferObject.hpp"
#include "Debug.hpp"

void VertexBufferObject::create(const UInt location, const Float* data, const UInt byteSize, const MeshDrawMode usage, const Index components)
{
	if(! loaded)
	{
		this->location = location;
		this->components = components;
		this->byteSize = byteSize;
		this->usage = usage;
		apiGenBuffer(bufferId);
		apiBindBuffer(GL_ARRAY_BUFFER, bufferId);
		apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		loaded = true;
	}
	else
	{
		logError("VBO already loaded");
	}
}

void VertexBufferObject::update(const Float* data, const UInt byteSize)
{
	if(loaded)
	{
		this->byteSize = byteSize;
		apiBindBuffer(GL_ARRAY_BUFFER, bufferId);
		apiBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
		loaded = true;
	}
}

void VertexBufferObject::destroy()
{
	if(loaded)
	{
		apiBindBuffer(GL_ARRAY_BUFFER, 0);
		apiDeleteBuffer(bufferId);
		loaded = false;
	}
}

void VertexBufferObject::attach() const
{
	apiEnableVertexAttributeArray(location);
	apiVertexAttributePointer(location, components, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void VertexBufferObject::detach() const
{
	apiDisableVertexAttributeArray(location);
}