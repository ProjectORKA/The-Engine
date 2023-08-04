#include "IndexBufferObject.hpp"
#include "GraphicsAPI.hpp"

void IndexBuffer::update(const Vector<UInt>& indices)
{
	indexCount = static_cast<Int>(indices.size());
	OpenGLIndexBuffer::update(indices);
}

void IndexBuffer::create(const Vector<UInt>& indices, const BufferUsage usage, const String& name)
{
	indexCount = static_cast<Int>(indices.size());
	OpenGLIndexBuffer::create(indices, usage, name);
}
