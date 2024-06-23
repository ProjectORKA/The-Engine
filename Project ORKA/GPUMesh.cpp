#include "GPUMesh.hpp"
#include "Uniforms.hpp"

void GPUMesh::unload()
{
	// make unavailable for rendering
	if (loaded)
	{
		vao.destroy();
		positionBuffer.destroy();
		textureCoordinateBuffer.destroy();
		normalBuffer.destroy();
		transformBuffer.destroy();
		indexBuffer.destroy();

		loaded = false;
	}
}

Bool GPUMesh::isLoaded() const
{
	return loaded;
}

void GPUMesh::upload(const CPUMesh& cpuMesh)
{
	if (loaded)
	{
		logWarning("Mesh already loaded!");
		return;
	}

	if (!cpuMesh.isLoaded())
	{
		logWarning("CPUMesh not loaded! Cant upload!");
		return;
	}

	primitiveMode = cpuMesh.primitiveMode;

	vao.create();

	if (cpuMesh.hasPositions())
	{
		positionBuffer.create(cpuMesh.positions, BufferUsage::StaticDraw, "Vertex Position Buffer");
		vao.bindVertexBuffer(0, positionBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	}

	if (cpuMesh.hasTextureCoordinates())
	{
		textureCoordinateBuffer.create(cpuMesh.textureCoordinates, BufferUsage::StaticDraw, "Texture Coordinate Vertex Buffer");
		vao.bindVertexBuffer(1, textureCoordinateBuffer.getID(), 0, sizeof(Vec2), 0, 2, DataType::Float, false, 0);
	}

	if (cpuMesh.hasNormals())
	{
		normalBuffer.create(cpuMesh.normals, BufferUsage::StaticDraw, "Vertex Normals Buffer");
		vao.bindVertexBuffer(2, normalBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasTangents())
	{
		tangentBuffer.create(cpuMesh.tangents, BufferUsage::StaticDraw, "Vertex Tangent Buffer");
		vao.bindVertexBuffer(3, tangentBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasBiTangents())
	{
		biTangentBuffer.create(cpuMesh.biTangents, BufferUsage::StaticDraw, "Vertex Bitangent Buffer");
		vao.bindVertexBuffer(4, biTangentBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasVertexColors())
	{
		vertexColorBuffer.create(cpuMesh.vertexColors, BufferUsage::StaticDraw, "Vertex Color Buffer");
		vao.bindVertexBuffer(5, vertexColorBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	}

	const Vector<Matrix> transforms = {Matrix(1)};
	transformBuffer.create(transforms, BufferUsage::DynamicDraw, "Transform Buffer");

	vao.bindVertexBuffer(6, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 0);
	vao.bindVertexBuffer(7, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 16);
	vao.bindVertexBuffer(8, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 32);
	vao.bindVertexBuffer(9, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 48);

	indexBuffer.create(cpuMesh.indices, BufferUsage::StaticDraw, "Index Buffer");
	vao.bindIndexBuffer(indexBuffer.getID());

	loaded = true;
}

void GPUMesh::render(Uniforms& uniforms) const
{
	if (loaded)
	{
		uniforms.setInstanced(false);
		uniforms.upload();
		vao.bind();
		vao.render(primitiveMode, indexBuffer.indexCount, nullptr);
	}
}

void GPUMesh::updateNormalBuffer(const Vector<Vec3>& normals) const
{
	if (loaded) normalBuffer.update(normals);
}

void GPUMesh::updateIndexBuffer(const Vector<unsigned int>& indices)
{
	if (loaded) indexBuffer.update(indices);
}

void GPUMesh::updateTangentBuffer(const Vector<Vec3>& tangents) const
{
	if (loaded) tangentBuffer.update(tangents);
}

void GPUMesh::updatePositionBuffer(const Vector<Vec3>& positions) const
{
	if (loaded) positionBuffer.update(positions);
}

void GPUMesh::updateBiTangentBuffer(const Vector<Vec3>& biTangents) const
{
	if (loaded) biTangentBuffer.update(biTangents);
}

void GPUMesh::updateVertexColorBuffer(const Vector<Vec3>& vertexColors) const
{
	if (loaded) vertexColorBuffer.update(vertexColors);
}

void GPUMesh::updateTextureCoordinateBuffer(const Vector<Vec2>& textureCoordinates) const
{
	if (loaded) textureCoordinateBuffer.update(textureCoordinates);
}

void GPUMesh::renderInstances(Uniforms& uniforms, const Vector<Matrix>& transforms) const
{
	if (loaded)
	{
		transformBuffer.update(transforms);
		uniforms.setInstanced(true);
		uniforms.upload();
		vao.bind();
		vao.renderInstanced(primitiveMode, indexBuffer.indexCount, nullptr, toIntSafe(transforms.size()));
	}
}
