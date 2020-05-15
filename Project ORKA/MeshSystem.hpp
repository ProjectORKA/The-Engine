#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "ProceduralMeshes.hpp"

struct MeshSystem {
	Vector<GPUMesh>		gpuMeshes;	//uploaded meshes ready to be drawn
	Map<Index, Index> renderComponentIndexToGPUMeshIndex;
	Map<String, Index> meshNames;

	void create();
	void destroy();

	//mesh
	void loadMesh(String name);
	void addMesh(CPUMesh& cpuMesh);
	void renderMesh(Index meshID);
	void renderMesh(String meshName);
};