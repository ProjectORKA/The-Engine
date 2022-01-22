
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct VertexBufferObject {
	UInt byteSize = 0;
	Index bufferID = 0;
	Index location = 0;
	Byte components = 3;
	Bool loaded = false;
	UInt usage = GL_STATIC_DRAW;

	void attach();
	void detach();
	void destroy();
	void update(Float* data, UInt byteSize);
	void create(UInt location, Float* data, UInt byteSize, UInt usage, Index components);
};