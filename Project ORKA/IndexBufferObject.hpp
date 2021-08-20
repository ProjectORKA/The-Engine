
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct IndexBufferObject {
	Index bufferID;
	UInt indexCount;
	void create(Index* data, UInt indexCount, UInt usage);
	void update(Index* data, UInt indexCount, UInt usage);
	void unload();
};

