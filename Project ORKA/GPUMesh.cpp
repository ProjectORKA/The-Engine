
#include "GPUMesh.hpp"
#include "Uniforms.hpp"

void GPUMesh::unload() {
	//make unavailable for rendering
	if (loaded) {
		vao.unload();
		loaded = false;
	}
}
void GPUMesh::upload(CPUMesh cpuMesh) {
	if (!loaded) {
		if (cpuMesh.loaded) {
			if (cpuMesh.positions.size() > 0) {
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
void GPUMesh::render(Uniforms& uniforms) {
	if (loaded) {
		uniforms.instanced(false);
		uniforms.upload();
		vao.select();
		apiDrawElements(primitiveMode, vao.indexBuffer.indexCount, GL_UNSIGNED_INT, nullptr);
	}
}
void GPUMesh::renderInstances(Uniforms& uniforms, UInt instanceCount) {
	if (loaded) {
		uniforms.instanced(true);
		uniforms.upload();
		vao.select();
		apiDrawElementsInstanced(primitiveMode, vao.indexBuffer.indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
	}
}