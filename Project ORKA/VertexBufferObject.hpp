#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct VertexBufferObject
{
	UInt byteSize = 0;
	Index bufferId = 0;
	Index location = 0;
	Byte components = 3;
	Bool loaded = false;
	MeshDrawMode usage = MeshDrawMode::StaticMode;

	void destroy();
	void attach() const;
	void detach() const;
	void update(const Float* data, UInt byteSize);
	void create(UInt location, const Float* data, UInt byteSize, MeshDrawMode usage, Index components);
};