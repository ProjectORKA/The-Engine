
#include "GPUMesh.hpp"
#include "Uniforms.hpp"

void GPUMesh::render(Uniforms & uniforms) {
	if (loaded) {
		uniforms.upload();
		vao.render();
		apiDrawElements(
		enumClassAsInt(primitiveMode),
			vao.indexBuffer.indexCount,
			GL_UNSIGNED_INT,
			(void*)0
		);
	}
}
void GPUMesh::unload() {
	//make unavailable for rendering
	if (loaded) {
		loaded = false;
		vao.unload();
	}
}
void GPUMesh::upload(CPUMesh cpuMesh) {
	if (!loaded) {
		if (cpuMesh.readyForUpload) {
			if (cpuMesh.vertices.size() > 0) {
				primitiveMode = cpuMesh.primitiveMode;

				vao.create(cpuMesh);

				loaded = true;
			}
		}
		else {
			logDebug("CPUMesh not loaded! Cant upload!");
		}
	}
	else {
		logDebug("GPUMesh already loaded!");
	}
}
