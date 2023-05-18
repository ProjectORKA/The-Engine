#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct IndexBufferObject {
	Index bufferID = -1;
	UInt indexCount = 0;
	void create(const Index* data, UInt indexCount, UInt usage);
	void update(const Index* data, UInt indexCount, UInt usage);
	void unload() const;
};
