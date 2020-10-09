
#include "GPUMesh.hpp"
//#include "Renderer.hpp"

void GPUMesh::upload(CPUMesh& cpuMesh) {
	if (!loaded) {
		if (cpuMesh.readyForUpload) {

			primitiveMode = cpuMesh.primitiveMode;

			vao.create(cpuMesh);

			loaded = true;
		}
	}
	else {
		logError("CPUMesh not loaded!");
	}
}
void GPUMesh::render() {
	if (loaded) {
#ifdef GRAPHICS_API_OPENGL

		vao.render();

		//render
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
#endif // GRAPHICS_API_OPENGL
}
void GPUMesh::unload() {
	//make unavailable for rendering
	loaded = false;
	vao.unload();
}