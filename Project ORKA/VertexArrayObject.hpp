
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"
#include "IndexBufferObject.hpp"
#include "VertexBufferObject.hpp"
#include "CPUMesh.hpp"

struct VertexArrayObject {
	Index arrayObjectID = 0;
	Bool instanced = false;
	Vector<VertexBufferObject> buffers;
	IndexBufferObject indexBuffer;
	void select();
	void unload();
	void create(CPUMesh& mesh);
	void add(Index location, float* data, UInt byteSize, UInt usage, Index components);
};