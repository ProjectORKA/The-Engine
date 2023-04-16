
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
	if (mesh.loaded) {
		apiGenVertexArray(arrayObjectID);
		apiBindVertexArray(arrayObjectID);

		if(mesh.dataFlags & MeshData::Positions)
			add(0, glm::value_ptr(mesh.positions[0]), mesh.positions.size() * sizeof(Vec3), mesh.drawMode, 3);

		if(mesh.dataFlags & MeshData::TextureCoords)
			add(1, glm::value_ptr(mesh.textureCoordinates[0]), mesh.textureCoordinates.size() * sizeof(Vec2), mesh.drawMode, 2);

		if(mesh.dataFlags & MeshData::Normals)
			add(2, glm::value_ptr(mesh.normals[0]), mesh.normals.size() * sizeof(Vec3), mesh.drawMode, 3);

		if(mesh.dataFlags & MeshData::VertexColor)
			add(3, glm::value_ptr(mesh.vertexColors[0]), mesh.vertexColors.size() * sizeof(Vec3), mesh.drawMode, 3);

		indexBuffer.create(mesh.indices.data(), mesh.indices.size(), mesh.drawMode);
	}
}
void VertexArrayObject::add(Index location, float* data, UInt byteSize, UInt usage, Index components)
{
	if (byteSize) {
		buffers.emplaceBack().create(location, data, byteSize, usage, components);
		buffers.last().attach();
	}
}