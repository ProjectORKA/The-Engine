#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct GPUMesh
{
	Bool loaded = false;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;

	VertexArrayObject vao;

	void render();
	void unload();
	void upload(CPUMesh& cpuMesh);
	//void update(CPUMesh& cpuMesh);
};