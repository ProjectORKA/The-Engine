
#include "GPUMesh.hpp"

void GPUMesh::upload(CPUMesh& cpuMesh) {
	if (!loaded) {
		if (cpuMesh.readyForUpload) {
			indexCount = cpuMesh.indices.size();
			primitiveMode = cpuMesh.primitiveMode;

			//create vertex array
#ifdef GRAPHICS_API_OPENGL
			glGenVertexArrays(1, &vertexArrayObjectID);
			glBindVertexArray(vertexArrayObjectID);
			//create vertex buffer
			glGenBuffers(1, &vertexBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, cpuMesh.vertices.size() * sizeof(glm::vec3), cpuMesh.vertices.data(), GL_STATIC_DRAW);

			//create uv buffer
			glGenBuffers(1, &uvBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
			glBufferData(GL_ARRAY_BUFFER, cpuMesh.uvs.size() * sizeof(glm::vec2), cpuMesh.uvs.data(), GL_STATIC_DRAW);

			//create index buffer
			glGenBuffers(1, &indexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, cpuMesh.indices.size() * sizeof(unsigned int), cpuMesh.indices.data(), GL_STATIC_DRAW);
#endif // GRAPHICS_API_OPENGL
			loaded = true;
		}
	}
	else {
		debugPrint("Error: CPUMesh not loaded!");
	}
}

void GPUMesh::render() {
	if (loaded) {
#ifdef GRAPHICS_API_OPENGL

		//bind
		glBindVertexArray(vertexArrayObjectID);
		//vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//uvs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		//render
		glDrawElements(
			primitiveMode,
			indexCount,
			GL_UNSIGNED_INT,
			(void*)0
		);

		//unbind
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	else {
		debugPrint("Error: GPUMesh not loaded!");
	}
#endif // GRAPHICS_API_OPENGL
}

void GPUMesh::unload() {
	//make unavailable for rendering
	loaded = false;
#ifdef GRAPHICS_API_OPENGL
	//delete buffers

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteBuffers(1, &uvBufferID);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBufferID);

	//delete vertex array object
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vertexArrayObjectID);
#endif // GRAPHICS_API_OPENGL
}