
#include "MeshSystem.hpp"

void MeshSystem::create() {
	CPUMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);


	CPUMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeCubeMesh(boundingBox, 0,1);
	addMesh(boundingBox);
}
void MeshSystem::destroy()
{
	for (GPUMesh& gpuMesh : gpuMeshes) {
		gpuMesh.unload();
	}
	gpuMeshes.clear();
	meshNames.clear();
}

void MeshSystem::loadMesh(Name name)
{
	CPUMesh mesh;
	mesh.autoLoadFromFile(name);

	if (mesh.readyForUpload) {
		addMesh(mesh);
	}
	else {

	}
}
void MeshSystem::addMesh(CPUMesh& cpuMesh) {
	gpuMeshes.emplace_back();
	gpuMeshes.back().upload(cpuMesh);
	meshNames[cpuMesh.name] = gpuMeshes.size() - 1;
}
void MeshSystem::renderMesh(Index meshID) {
	gpuMeshes[meshID].render();
}
void MeshSystem::renderMesh(Name meshName) {
	auto it = meshNames.find(meshName);
	if (it != meshNames.end()) {
		gpuMeshes[it->second].render();
	}
	else {
		logDebug("Mesh not loaded.");
	}
}