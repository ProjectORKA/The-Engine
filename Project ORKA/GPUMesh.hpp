#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct Uniforms;

struct GPUMesh {
	Bool loaded = false;
	VertexArrayObject vao;
	Int drawMode = MeshDrawMode::staticMode;
	Int primitiveMode = PrimitiveMode::Triangles;

	void unload();
	void upload(CpuMesh cpuMesh);
	void render(Uniforms& uniforms);
	void renderInstances(Uniforms& uniforms, UInt instanceCount);
};
