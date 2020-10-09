
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct VertexBufferObject {
	Index bufferID = 0;
	Index location = 0;
	Byte components = 3;
	UInt byteSize = 0;

	void create(UInt location, float* data, UInt byteSize, UInt usage, Index components);
	void unload();
};