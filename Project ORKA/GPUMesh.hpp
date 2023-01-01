
#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct Uniforms;

struct GPUMesh
{
	Bool loaded = false;
	VertexArrayObject vao;
	Int drawMode = MeshDrawMode::staticMode;
	Int primitiveMode = PrimitiveMode::Triangles;

	void unload();
	void upload(CPUMesh cpuMesh);
	void render(Uniforms& uniforms);
	void renderInstances(Uniforms& uniforms, UInt instanceCount);
};