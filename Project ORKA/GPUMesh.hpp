#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct GPUMesh {
	Bool loaded = false;
	UInt primitiveMode = Triangles;

	VertexArrayObject vao;

	void upload(CPUMesh& cpuMesh);
	void render();
	void unload();
};