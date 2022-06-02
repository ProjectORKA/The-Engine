
#include "VertexArrayObject.hpp"

void VertexArrayObject::unload()
{
	for (auto& buffer : buffers) {
		buffer.destroy();
	}
	buffers.clear();
	indexBuffer.unload();
	apiDeleteVertexArray(arrayObjectID);
}
void VertexArrayObject::select()
{
	apiBindVertexArray(arrayObjectID);
}
void VertexArrayObject::create(CPUMesh& mesh)
{
	if (mesh.readyForUpload) {
		apiGenVertexArray(arrayObjectID);
		apiBindVertexArray(arrayObjectID);

		add(0, glm::value_ptr(mesh.vertices[0]), mesh.vertices.size() * sizeof(Vec3), mesh.drawMode, 3);
		add(1, glm::value_ptr(mesh.uvs[0]), mesh.uvs.size() * sizeof(Vec2), mesh.drawMode, 2);
		add(2, glm::value_ptr(mesh.normals[0]), mesh.normals.size() * sizeof(Vec3), mesh.drawMode, 3);
		add(3, glm::value_ptr(mesh.colors[0]), mesh.colors.size() * sizeof(Vec3), mesh.drawMode, 3);

		indexBuffer.create(mesh.indices.data(), mesh.indices.size(), mesh.drawMode);
	}
}
void VertexArrayObject::add(Index location, float* data, UInt byteSize, UInt usage, Index components)
{
	buffers.emplace_back().create(location, data, byteSize, usage, components);
	buffers.back().attach();
}