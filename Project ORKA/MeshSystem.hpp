#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct Uniforms;

struct MeshSystem {
	Vector<GPUMesh>		gpuMeshes;	//uploaded meshes ready to be drawn
	Map<Name, Index> meshNames;

	Index currentMeshID = 0;

	void create();
	void destroy();

	//mesh
	void use(Name name);
	void use(Index meshID);
	void loadMesh(Name name);
	void addMesh(CPUMesh cpuMesh);
	void render(Uniforms& uniforms, Index meshID);
	void render(Uniforms& uniforms, Name meshName);
	void renderInstanced(Uniforms& uniforms, Name meshName, Vector<Vec4>& transformations);

	GPUMesh& currentMesh();
};