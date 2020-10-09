
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct IndexBufferObject {
	Index bufferID;
	UInt indexCount;
	void create(Index* data, UInt byteSize, UInt usage);
	void unload();
};

