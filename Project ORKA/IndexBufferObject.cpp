#include "IndexBufferObject.hpp"
#include "GraphicsAPI.hpp"

void IndexBuffer::update(OpenGlContext& openGlContext, const Vector<UInt>& indices)
{
	indexCount = static_cast<Int>(indices.size());
	OpenGLIndexBuffer::update(openGlContext, indices);
}

void IndexBuffer::create(OpenGlContext& openGlContext, const Vector<UInt>& indices, const BufferUsage usage, const String& name)
{
	indexCount = static_cast<Int>(indices.size());
	OpenGLIndexBuffer::create(openGlContext, indices, usage, name);
}
