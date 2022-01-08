
#include "MeshSystem.hpp"
#include "Uniforms.hpp"

void MeshSystem::create() {
	CPUMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);

	CPUMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeAxisLines(boundingBox);
	addMesh(boundingBox);

	CPUMesh fullScreenQuad;
	fullScreenQuad.load("fullScreenQuad");
	addMesh(fullScreenQuad);
}
void MeshSystem::destroy()
{
	for (GPUMesh& gpuMesh : gpuMeshes) {
		gpuMesh.unload();
	}
	gpuMeshes.clear();
	meshNames.clear();
}
void MeshSystem::use(Index meshID)
{
	currentMeshID = meshID;
}
void MeshSystem::use(Name meshName)
{
	auto it = meshNames.find(meshName);
	if (it != meshNames.end()) {
		currentMeshID = it->second;
	}
	else {
		loadMesh(meshName);
		it = meshNames.find(meshName);
		if (it != meshNames.end()) {
			currentMeshID = it->second;
		}
		else {
			logError("Could not select nor load mesh!");
		}
	}
}
void MeshSystem::loadMesh(Name name)
{
	CPUMesh mesh;
	mesh.load(name);

	if (mesh.readyForUpload) {
		addMesh(mesh);
	}
	else {
		logDebug("Mesh could not be loaded from file!");
	}
}
void MeshSystem::render(Uniforms& uniforms, Index meshID) {
	use(meshID);
	currentMesh().render(uniforms);
}
void MeshSystem::addMesh(CPUMesh cpuMesh) {
	//[TODO] check if it works
	GPUMesh gpuMesh;
	gpuMesh.upload(cpuMesh);
	
	gpuMeshes.push_back(gpuMesh);

	use(gpuMeshes.size() - 1);
	//currentMesh().upload(cpuMesh);
	meshNames[cpuMesh.name] = currentMeshID;
}
void MeshSystem::render(Uniforms & uniforms, Name meshName) {
	use(meshName);
	currentMesh().render(uniforms);
}

GPUMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshID];
}