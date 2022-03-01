
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
void GPUMesh::render(Uniforms& uniforms) {
	if (loaded) {
		uniforms.instanced(false);
		uniforms.upload();
		vao.select();
		if (instanced) {
			transformations.detach();
			instanced = false;
		}
		apiDrawElements(enumClassAsInt(primitiveMode), vao.indexBuffer.indexCount, GL_UNSIGNED_INT, nullptr);
	}
}
void GPUMesh::renderInstances(Uniforms& uniforms, Vector<Vec4>& data) {
	if (loaded) {
		uniforms.instanced(true);
		uniforms.upload();
		if (transformations.loaded) {
			vao.select();
			transformations.update(glm::value_ptr(data[0]), data.size() * sizeof(Vec4));
			transformations.attach();
		}
		else {
			vao.select();
			transformations.create(3, glm::value_ptr(data[0]), sizeof(Vec4) * data.size(), GL_STATIC_DRAW, 4);
			glVertexAttribDivisor(3, 1);
			transformations.attach();
		}

		instanced = true;

		apiDrawElementsInstanced(enumClassAsInt(primitiveMode), vao.indexBuffer.indexCount, GL_UNSIGNED_INT, nullptr, data.size());
	}
}