#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Scene.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct Uniforms;

struct NameTable{
	Vector<Name> names;
	Vector<Index> indices;

	void add(Name name, Index id);
	Bool find(Name name, Index& id);
};


struct MeshSystem {
	VertexBufferObject transforms;

	Vector<GPUMesh> gpuMeshes;	//uploaded meshes ready to be drawn
	NameTable meshNames;

	Index currentMeshID = 0;

	void create();
	void destroy();

	//mesh
	void use(Name name);
	void use(Index meshID);
	void addMesh(CPUMesh cpuMesh);
	void render(Uniforms& uniforms, Index meshID);
	void render(Uniforms& uniforms, Name meshName);
	void renderInstanced(Uniforms& uniforms, Name meshName, Vector<Matrix>& transformations);

	GPUMesh& currentMesh();
};