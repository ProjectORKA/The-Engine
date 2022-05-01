#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct Uniforms;

struct GPUMesh
{
	Bool loaded = false;
	VertexArrayObject vao;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	void unload();
	void upload(CPUMesh cpuMesh);
	void render(Uniforms& uniforms);
	void renderInstances(Uniforms& uniforms, UInt instanceCount);
};