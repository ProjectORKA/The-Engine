#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "BasicsModule.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct MeshSystem {
	Vector<GPUMesh>		gpuMeshes;	//uploaded meshes ready to be drawn
	Map<Name, Index> meshNames;

	void create();
	void destroy();

	//mesh
	void loadMesh(Name name);
	void addMesh(CPUMesh& cpuMesh);
	void renderMesh(Index meshID);
	void renderMesh(Name meshName);
};