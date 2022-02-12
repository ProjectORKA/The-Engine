
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
}
void MeshSystem::use(Index meshID)
{
	currentMeshID = meshID;
}
void MeshSystem::use(Name meshName)
{
	Index id;
	if (meshNames.find(meshName, id)) {
		currentMeshID = id;
	}
	else {
		loadMesh(meshName);
		if (meshNames.find(meshName, id)) {
			currentMeshID = id;
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
void MeshSystem::addMesh(CPUMesh cpuMesh) {
	//[TODO] check if it works
	GPUMesh gpuMesh;
	gpuMesh.upload(cpuMesh);
	gpuMeshes.push_back(gpuMesh);
	use(gpuMeshes.size() - 1);
	meshNames.add(cpuMesh.name,currentMeshID);
}
void MeshSystem::render(Uniforms& uniforms, Index meshID) {
	use(meshID);
	currentMesh().render(uniforms);
}
void MeshSystem::render(Uniforms & uniforms, Name meshName) {
	use(meshName);
	currentMesh().render(uniforms);
}
void MeshSystem::renderInstanced(Uniforms& uniforms, Name meshName, Vector<Vec4>& transformations) {
	use(meshName);
	currentMesh().renderInstances(uniforms,transformations);
}

GPUMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshID];
}