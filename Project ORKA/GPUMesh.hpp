#pragma once

#include "Debug.hpp"
#include "VertexArrayObject.hpp"

struct Uniforms;

struct GPUMesh
{
	Bool loaded = false;
	VertexArrayObject vao;
	VertexBufferObject transformations;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	Bool instanced = false;

	void unload();
	void upload(CPUMesh cpuMesh);
	void render(Uniforms& uniforms);
	void renderInstances(Uniforms& uniforms, Vector<Vec4>& data);
};