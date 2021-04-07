
#include "VertexArrayObject.hpp"



void VertexArrayObject::create(CPUMesh& mesh)
{
	if (mesh.readyForUpload) {
		glGenVertexArrays(1, &arrayObjectID);
		glBindVertexArray(arrayObjectID);

		add(0, glm::value_ptr(mesh.vertices[0]), mesh.vertices.size() * sizeof(Vec3), StaticBufferUsage, 3);
		add(1, glm::value_ptr(mesh.uvs[0]), mesh.uvs.size() * sizeof(Vec2), StaticBufferUsage, 2);
		add(2, glm::value_ptr(mesh.normals[0]), mesh.normals.size() * sizeof(Vec3), StaticBufferUsage, 3);

		indexBuffer.create(mesh.indices.data(), mesh.indices.size(), StaticBufferUsage);
	}
}

void VertexArrayObject::add(Index location, float* data, UInt byteSize, UInt usage, Index components)
{
	buffers.emplace_back();
	buffers.back().create(location, data, byteSize, usage, components);
}

void VertexArrayObject::render()
{
	glBindVertexArray(arrayObjectID);
}

void VertexArrayObject::unload()
{
	for (auto& buffer : buffers) {
		buffer.unload();
	}
	buffers.clear();
	indexBuffer.unload();
	glDeleteVertexArrays(1, &arrayObjectID);
}