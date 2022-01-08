#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct Uniforms;

struct GPUMesh
{
	Bool loaded = false;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;

	VertexArrayObject vao;

	void unload();
	void upload(CPUMesh cpuMesh);
	void render(Uniforms& uniforms);
};