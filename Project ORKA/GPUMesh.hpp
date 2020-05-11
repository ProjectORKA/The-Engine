#pragma once

#include "Debug.hpp"
#include "PrimitiveModes.hpp"
#include "CPUMesh.hpp"

struct GPUMesh {
	Bool loaded = false;
	Int indexCount = 0;
	Index vertexArrayObjectID = 0;
	Index vertexBufferID = 0;
	Index uvBufferID = 0;
	Index indexBufferID = 0;
	PrimitiveMode primitiveMode = Triangles;

	void upload(CPUMesh & cpuMesh);
	void render();
	void unload();
};