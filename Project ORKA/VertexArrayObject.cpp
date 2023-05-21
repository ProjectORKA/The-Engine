#include "VertexArrayObject.hpp"

void VertexArrayObject::unload()
{
	for(auto& buffer : buffers)
	{
		buffer.destroy();
	}
	buffers.clear();
	indexBuffer.unload();
	apiDeleteVertexArray(arrayObjectId);
}

void VertexArrayObject::select() const
{
	apiBindVertexArray(arrayObjectId);
}

void VertexArrayObject::create(CpuMesh& mesh)
{
	if(mesh.loaded)
	{
		apiGenVertexArray(arrayObjectId);
		apiBindVertexArray(arrayObjectId);

		if(mesh.dataFlags & Positions) add(0, value_ptr(mesh.positions[0]), mesh.positions.size() * sizeof(Vec3), mesh.drawMode, 3);
		if(mesh.dataFlags & TextureCoords) add(1, value_ptr(mesh.textureCoordinates[0]), mesh.textureCoordinates.size() * sizeof(Vec2), mesh.drawMode, 2);
		if(mesh.dataFlags & Normals) add(2, value_ptr(mesh.normals[0]), mesh.normals.size() * sizeof(Vec3), mesh.drawMode, 3);
		if(mesh.dataFlags & VertexColor) add(3, value_ptr(mesh.vertexColors[0]), mesh.vertexColors.size() * sizeof(Vec3), mesh.drawMode, 3);

		indexBuffer.create(mesh.indices.data(), mesh.indices.size(), mesh.drawMode);
	}
}

void VertexArrayObject::add(const Index location, const Float* data, const UInt byteSize, const MeshDrawMode usage, const Index components)
{
	if(byteSize)
	{
		buffers.emplace_back().create(location, data, byteSize, usage, components);
		buffers.back().attach();
	}
}