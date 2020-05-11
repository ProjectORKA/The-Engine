#pragma once
#include <iostream>
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "GPUMesh.hpp"
#include "MeshFileInfo.hpp"
#include "ProceduralMeshes.hpp"

struct MeshSystem {
	List<MeshFileInfo>	meshFiles;	//info to load meshes from files
	Vector<GPUMesh>		gpuMeshes;	//uploaded meshes ready to be drawn

	Map<Index, Index> renderComponentIndexToGPUMeshIndex;
	Map<String, Index> meshNames;

	void create();
	void destroy();

	//mesh
	void addMesh(CPUMesh& cpuMesh);
	void renderMesh(Index meshID);
	void renderMesh(String meshName);

	//meshfiles
	void addMeshFile(Path path);
	void loadFromMeshFile(String meshName);
	void loadFromMeshFile(MeshFileInfo & info);
};

//object folder
void loadFilesFromObjectFolder(MeshSystem& meshSystem);