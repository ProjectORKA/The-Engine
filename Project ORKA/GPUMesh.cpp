
#include "GPUMesh.hpp"

void GPUMesh::render() {
	if (loaded) {
		//bind vertex data
		vao.render();
		//render vertex data
		glDrawElements(
			primitiveMode,
			vao.indexBuffer.indexCount,
			GL_UNSIGNED_INT,
			(void*)0
		);
	}
	else {
		logError("GPUMesh not loaded!");
	}
}
void GPUMesh::unload() {
	//make unavailable for rendering
	if (loaded) {
		loaded = false;
		vao.unload();
	}
}
void GPUMesh::upload(CPUMesh& cpuMesh) {
	if (!loaded) {
		if (cpuMesh.readyForUpload) {

			primitiveMode = cpuMesh.primitiveMode;

			vao.create(cpuMesh);

			loaded = true;
		}
		else {
			logError("CPUMesh not loaded! Cant upload!");
		}
	}
	else {
		logError("GPUMesh already loaded!");
	}
}