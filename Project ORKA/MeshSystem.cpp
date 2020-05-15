
#include "MeshSystem.hpp"

void MeshSystem::create() {
	//loadFilesFromObjectFolder(*this);

	CPUMesh plane;
	plane.name = "plane";
	proceduralPlaneMesh(plane, 0.75, 0.75);
	addMesh(plane);

	CPUMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);

	CPUMesh boundingBox;
	boundingBox.name = "bounding box";
	proceduralWireframeCubeMesh(boundingBox, 0.5);
	addMesh(boundingBox);
}
void MeshSystem::destroy()
{
	for (GPUMesh& gpuMesh : gpuMeshes) {
		gpuMesh.unload();
	}
	gpuMeshes.clear();
	//meshFiles.clear();
	renderComponentIndexToGPUMeshIndex.clear();
	meshNames.clear();
}

void MeshSystem::loadMesh(String name)
{
	Path meshFilePath(String("Data/meshes/").append(name).append(".mesh"));
	CPUMesh mesh;
	mesh.loadMeshFile(meshFilePath);

	if (mesh.readyForUpload) {
		addMesh(mesh);
	}
	else {
		Path fbxFilePath(String("Data/objects/").append(name).append(".fbx"));
		mesh.loadFBX(name, Triangles, fbxFilePath);

		if (mesh.readyForUpload) {
			addMesh(mesh);
		}
		else {
			logDebug(String("Mesh with the name (").append(name).append(") does not exist!"));
		}
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
void MeshSystem::renderMesh(String meshName) {
	auto it = meshNames.find(meshName);
	if (it != meshNames.end()) {
		gpuMeshes[it->second].render();
	}
	else {
		logDebug("Mesh not loaded.");
	}
}

//void MeshSystem::addMeshFile(Path path) {
//	if (path.extension() == ".fbx") {
//		MeshFileInfo info;
//		info.name = path.filename().string();
//		info.name = info.name.erase(info.name.size() - 4);
//		info.path = path.string();
//		info.primitiveMode = Triangles;
//#ifdef DEBUG
//		std::cout << "(" << info.name << ")		" << info.path << "\n";
//#endif // DEBUG
//		meshFiles.push_back(info);
//	}
//};
//void MeshSystem::loadFromMeshFile(String meshName) {
//	for (MeshFileInfo info : meshFiles) {
//		if (info.name == meshName) {
//			CPUMesh cpuMesh;
//			cpuMesh.load(info);
//			addMesh(cpuMesh);
//			return;
//		}
//	}
//}
//void MeshSystem::loadFromMeshFile(MeshFileInfo & info) {
//	CPUMesh cpuMesh;
//	cpuMesh.load(info);
//	addMesh(cpuMesh);
//}

//void loadFilesFromObjectFolder(MeshSystem& meshSystem) {
//	if (std::filesystem::exists(std::filesystem::current_path() / "Data/objects"))
//		for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / "Data/objects")) {
//			meshSystem.addMeshFile(entry.path());
//		}
//	else debugPrint("Error: MeshFolder (Data/objects) doesent exist!");
//}