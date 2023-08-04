#pragma once

#include "GraphicsAPI.hpp"

struct IndexBuffer : OpenGLIndexBuffer
{
	Int indexCount = -1; // needs to be Int because of glDrawElements

	void update(const Vector<UInt>& indices);
	void create(const Vector<UInt>& indices, BufferUsage usage, const String& name);
};
