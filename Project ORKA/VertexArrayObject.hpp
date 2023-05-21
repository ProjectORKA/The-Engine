#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"
#include "IndexBufferObject.hpp"
#include "VertexBufferObject.hpp"
#include "CpuMesh.hpp"

struct VertexArrayObject
{
	Bool instanced = false;
	Index arrayObjectId = 0;
	IndexBufferObject indexBuffer;
	Vector<VertexBufferObject> buffers;

	void unload();
	void select() const;
	void create(CpuMesh& mesh);
	void add(Index location, const Float* data, UInt byteSize, MeshDrawMode usage, Index components);
};