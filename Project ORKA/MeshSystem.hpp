#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

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
	void renderMesh(Index meshID);
	void addMesh(CPUMesh& cpuMesh);
	void renderMesh(Name meshName);

	GPUMesh& currentMesh();
};