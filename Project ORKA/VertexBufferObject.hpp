#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct VertexBufferObject {
	UInt byteSize = 0;
	Index bufferId = 0;
	Index location = 0;
	Byte components = 3;
	Bool loaded = false;
	UInt usage = GL_STATIC_DRAW;

	void attach() const;
	void detach() const;
	void destroy();
	void update(const Float* data, UInt byteSize);
	void create(UInt location, const Float* data, UInt byteSize, UInt usage, Index components);
};
