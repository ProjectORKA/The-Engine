
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
	if (cpuMesh.readyForUpload) {
		GPUMesh gpuMesh;
		gpuMesh.upload(cpuMesh);
		gpuMeshes.push_back(gpuMesh);
		use(gpuMeshes.size() - 1);
		meshNames.add(cpuMesh.name,currentMeshID);
	}
	else {
		logError("Mesh could not be loaded properly!");
	}
}
void MeshSystem::render(Uniforms& uniforms, Index meshID) {
	use(meshID);
	currentMesh().render(uniforms);
}
void MeshSystem::render(Uniforms & uniforms, Name meshName) {
	use(meshName);
	currentMesh().render(uniforms);
}
void MeshSystem::renderInstanced(Uniforms& uniforms, Name meshName, Vector<Matrix>& data) {
	use(meshName);

	currentMesh().vao.select();

	if (currentMesh().vao.instanced) {
		transforms.update(glm::value_ptr(data[0]), data.size() * sizeof(Matrix));
	}
	else {
		if (!transforms.loaded){
			transforms.create(3, glm::value_ptr(data[0]), data.size() * sizeof(Matrix), GL_STATIC_DRAW, 4);
		}
		apiBindBuffer(GL_ARRAY_BUFFER, transforms.bufferID);
		
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)0);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(1 * sizeof(Vec4)));
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(2 * sizeof(Vec4)));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (void*)(3 * sizeof(Vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		currentMesh().vao.instanced = true;
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
