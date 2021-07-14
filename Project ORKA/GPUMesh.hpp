#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct GPUMesh
{
	Bool loaded = false;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;

	VertexArrayObject vao;

	void upload(CPUMesh& cpuMesh);
	void render();
	void unload();
};