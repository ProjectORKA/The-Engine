#include "MeshSystem.hpp"
#include "Uniforms.hpp"

void MeshSystem::create(ResourceManager& resourceManager) {
	CpuMesh standard;
	standard.name = "default";
	proceduralPlaneMesh(standard, 1, 1);
	addMesh(standard);

	CpuMesh boundingBox;
	boundingBox.name = "boundingBox";
	proceduralWireframeAxisLines(boundingBox);
	addMesh(boundingBox);

	basicMeshes.create(resourceManager);
}

void MeshSystem::destroy() {
	for (GPUMesh& gpuMesh : gpuMeshes) { gpuMesh.unload(); }
	gpuMeshes.clear();
	basicMeshes.destroy();
}

void MeshSystem::use(const Index meshID) { currentMeshID = meshID; }

void MeshSystem::use(ResourceManager& resourceManager, const Name& meshName) {
	Index id;
	if (meshNames.find(meshName, id)) { currentMeshID = id; }
	else {
		CpuMesh mesh;
		mesh.load(resourceManager, meshName);
		addMesh(mesh);
		if (meshNames.find(meshName, id)) { currentMeshID = id; }
		else { logError("Mesh could not be loaded!"); }
	}
}

void MeshSystem::addMesh(const CpuMesh& cpuMesh) {
	if (cpuMesh.loaded) {
		GPUMesh gpuMesh;
		gpuMesh.upload(cpuMesh);
		gpuMeshes.push_back(gpuMesh);
		use(gpuMeshes.size() - 1);
		meshNames.add(cpuMesh.name, currentMeshID);
	}
	else { logWarning("Mesh could not be loaded!"); }
}

void MeshSystem::render(Uniforms& uniforms, const Index meshID) {
	use(meshID);
	currentMesh().render(uniforms);
}

void MeshSystem::render(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName) {
	use(resourceManager, meshName);
	currentMesh().render(uniforms);
}

void MeshSystem::renderInstanced(ResourceManager& resourceManager, Uniforms& uniforms, const Name& meshName,
                                 Vector<Matrix>& data) {
	use(resourceManager, meshName);

	const UInt transformPos = 4;

	if (!data.empty()) {
		currentMesh().vao.select();

		if (currentMesh().vao.instanced) { transforms.update(value_ptr(data[0]), data.size() * sizeof(Matrix)); }
		else {
			if (!transforms.loaded) {
				transforms.create(4, value_ptr(data[0]), data.size() * sizeof(Matrix), GL_STATIC_DRAW, 4);
			}
			apiBindBuffer(GL_ARRAY_BUFFER, transforms.bufferId);

			apiEnableVertexAttribArray(transformPos + 0);
			apiEnableVertexAttribArray(transformPos + 1);
			apiEnableVertexAttribArray(transformPos + 2);
			apiEnableVertexAttribArray(transformPos + 3);

			apiVertexAttribPointer(transformPos + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), nullptr);
			apiVertexAttribPointer(transformPos + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(1 * sizeof(Vec4)));
			apiVertexAttribPointer(transformPos + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(2 * sizeof(Vec4)));
			apiVertexAttribPointer(transformPos + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(3 * sizeof(Vec4)));

			apiVertexAttribDivisor(transformPos + 0, 1);
			apiVertexAttribDivisor(transformPos + 1, 1);
			apiVertexAttribDivisor(transformPos + 2, 1);
			apiVertexAttribDivisor(transformPos + 3, 1);

			currentMesh().vao.instanced = true;
		}
	}

	currentMesh().renderInstances(uniforms, data.size());
}

void MeshSystem::renderFullscreen(Uniforms& uniforms) { basicMeshes.fullscreenMesh.render(uniforms); }

GPUMesh& MeshSystem::currentMesh() { return gpuMeshes[currentMeshID]; }

void NameTable::add(const Name& name, const Index id) {
	names.push_back(name);
	indices.push_back(id);
}

Bool NameTable::find(const Name& name, Index& id) const {
	id = -1;
	for (Index i = 0; i < names.size(); i++) {
		if (names[i] == name) {
			id = indices[i];
			return true;
		}
	}
	return false;
}
