#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct IndexBufferObject
{
	Index bufferId = -1;
	ULL indexCount = 0;
	void create(const Index* data, ULL indexCount, MeshDrawMode usage);
	void update(const Index* data, ULL indexCount, MeshDrawMode usage);
	void unload() const;
};