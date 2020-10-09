
#include "VertexArrayObject.hpp"

void VertexArrayObject::create(CPUMesh& mesh)
{

#ifdef GRAPHICS_API_OPENGL
	glGenVertexArrays(1, &arrayObjectID);
	glBindVertexArray(arrayObjectID);
#endif // GRAPHICS_API_OPENGL

	add(0, glm::value_ptr(mesh.vertices[0]), mesh.vertices.size() * sizeof(Vec3), StaticBufferUsage, 3);
	add(1, glm::value_ptr(mesh.uvs[0]), mesh.uvs.size() * sizeof(Vec2), StaticBufferUsage, 2);
	add(2, glm::value_ptr(mesh.normals[0]), mesh.normals.size() * sizeof(Vec3), StaticBufferUsage, 3);

	indexBuffer.create(mesh.indices.data(), mesh.indices.size(), StaticBufferUsage);

}
void VertexArrayObject::add(Index location, float* data, UInt byteSize, UInt usage, Index components)
{
	buffers.emplace_back();
	buffers.back().create(location, data, byteSize, usage, components);
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