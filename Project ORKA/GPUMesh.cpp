
#include "GPUMesh.hpp"

#include "Renderer.hpp"

void VertexBufferObject::create(UInt location, float* data, UInt byteSize, UInt usage, Index components)
{
#ifdef GRAPHICS_API_OPENGL
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, byteSize, data, usage);
	glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0, (void*)0);
#endif // GRAPHICS_API_OPENGL
}
void VertexBufferObject::unload()
{
#ifdef GRAPHICS_API_OPENGL
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);
#endif // GRAPHICS_API_OPENGL
}

void IndexBufferObject::create(Index* data, UInt byteSize, UInt usage)
{
#ifdef GRAPHICS_API_OPENGL
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteSize, data, usage);
#endif // GRAPHICS_API_OPENGL
}
void IndexBufferObject::unload()
{
#ifdef GRAPHICS_API_OPENGL
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &bufferID);
#endif // GRAPHICS_API_OPENGL
}

void VertexArrayObject::create(CPUMesh& mesh)
{

#ifdef GRAPHICS_API_OPENGL
	glGenVertexArrays(1, &arrayObjectID);
	glBindVertexArray(arrayObjectID);
#endif // GRAPHICS_API_OPENGL

	add(glm::value_ptr(mesh.vertices[0]), mesh.vertices.size() * sizeof(Vec3), StaticBufferUsage, 3);
	add(glm::value_ptr(mesh.uvs[0]), mesh.uvs.size() * sizeof(Vec2), StaticBufferUsage, 2);

	indexBuffer.create(mesh.indices.data(), mesh.indices.size() * sizeof(Index), StaticBufferUsage);

}
void VertexArrayObject::add(float* data, UInt byteSize, UInt usage, Index components)
{
	buffers.emplace_back();
	buffers.back().create(vertexAttributeCount, data, byteSize, usage, components);
#ifdef GRAPHICS_API_OPENGL
	glEnableVertexAttribArray(vertexAttributeCount);
#endif // GRAPHICS_API_OPENGL
	vertexAttributeCount++;
}
void VertexArrayObject::render()
{
#ifdef GRAPHICS_API_OPENGL
	glBindVertexArray(arrayObjectID);

#endif // GRAPHICS_API_OPENGL
}
void VertexArrayObject::unload()
{
	for (auto& buffer : buffers) {
		buffer.unload();
	}
	buffers.clear();

	indexBuffer.unload();
#ifdef GRAPHICS_API_OPENGL
	//glBindVertexArray(0);
	glDeleteVertexArrays(1, &arrayObjectID);
#endif // GRAPHICS_API_OPENGL
}

void GPUMesh::upload(CPUMesh& cpuMesh) {
	if (!loaded) {
		if (cpuMesh.readyForUpload) {
			indexCount = cpuMesh.indices.size();
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
			indexCount,
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