
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct VertexBufferObject {
	Index bufferID = 0;
	Index location = 0;
	Byte components = 3;
	UInt byteSize = 0;
	bool loaded = false;


	void create(UInt location, float* data, UInt byteSize, UInt usage, Index components);
	//void update(UInt location, float* data, UInt byteSize, UInt usage, Index components);
	void unload();
};