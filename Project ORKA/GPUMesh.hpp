#pragma once

#include "VertexArrayObject.hpp"

struct Uniforms;

struct GpuMesh
{
	Bool loaded = false;
	VertexArrayObject vao;
	MeshDrawMode drawMode = MeshDrawMode::StaticMode;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	void unload();
	void upload(CpuMesh cpuMesh);
	void render(Uniforms& uniforms) const;
	void renderInstances(Uniforms& uniforms, UInt instanceCount) const;
};