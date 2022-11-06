
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
		CPUMesh mesh;
		mesh.load(meshName);
		addMesh(mesh);
		if (meshNames.find(meshName, id)) {
			currentMeshID = id;
		}
		else {
			logError("Mesh could not be loaded!");
		}
	}
}
void MeshSystem::addMesh(CPUMesh cpuMesh) {
	if (cpuMesh.loaded) {
		GPUMesh gpuMesh;
		gpuMesh.upload(cpuMesh);
		gpuMeshes.push_back(gpuMesh);
		use(gpuMeshes.size() - 1);
		meshNames.add(cpuMesh.name, currentMeshID);
	}
	else {
		logWarning("Mesh could not be loaded!");
	}
}
void MeshSystem::render(Uniforms& uniforms, Index meshID) {
	use(meshID);
	currentMesh().render(uniforms);
}
void MeshSystem::render(Uniforms& uniforms, Name meshName) {
	use(meshName);
	currentMesh().render(uniforms);
}
void MeshSystem::renderInstanced(Uniforms& uniforms, Name meshName, Vector<Matrix>& data) {
	use(meshName);

	const UInt transformPos = 4;

	if (data.size() > 0) {

		currentMesh().vao.select();

		if (currentMesh().vao.instanced) {
			transforms.update(glm::value_ptr(data[0]), data.size() * sizeof(Matrix));
		}
		else {
			if (!transforms.loaded) {
				transforms.create(4, glm::value_ptr(data[0]), data.size() * sizeof(Matrix), GL_STATIC_DRAW, 4);
			}
			apiBindBuffer(GL_ARRAY_BUFFER, transforms.bufferID);

			glEnableVertexAttribArray(transformPos + 0);
			glEnableVertexAttribArray(transformPos + 1);
			glEnableVertexAttribArray(transformPos + 2);
			glEnableVertexAttribArray(transformPos + 3);

			glVertexAttribPointer(transformPos + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)0);
			glVertexAttribPointer(transformPos + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(1 * sizeof(Vec4)));
			glVertexAttribPointer(transformPos + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(2 * sizeof(Vec4)));
			glVertexAttribPointer(transformPos + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(3 * sizeof(Vec4)));

			glVertexAttribDivisor(transformPos + 0, 1);
			glVertexAttribDivisor(transformPos + 1, 1);
			glVertexAttribDivisor(transformPos + 2, 1);
			glVertexAttribDivisor(transformPos + 3, 1);

			currentMesh().vao.instanced = true;
		}
	}

	currentMesh().renderInstances(uniforms, data.size());
}

GPUMesh& MeshSystem::currentMesh()
{
	return gpuMeshes[currentMeshID];
}

void NameTable::add(Name name, Index id) {
	names.push_back(name);
	indices.push_back(id);
}
Bool NameTable::find(Name name, Index& id) {
	id = -1;
	for (Index i = 0; i < names.size(); i++) {
		if (names[i] == name) {
			id = indices[i];
			return true;
		}
	}
	return false;
}
