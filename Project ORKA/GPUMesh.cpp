#include "GPUMesh.hpp"
#include "Uniforms.hpp"
#include "Renderer.hpp"

void GPUMesh::unload(Renderer& renderer)
{
	// make unavailable for rendering
	if (loaded)
	{
		vao.destroy(renderer.openGlContext);
		positionBuffer.destroy(renderer.openGlContext);
		textureCoordinateBuffer.destroy(renderer.openGlContext);
		vertexNormalBuffer.destroy(renderer.openGlContext);
		faceNormalBuffer.destroy(renderer.openGlContext);
		transformBuffer.destroy(renderer.openGlContext);
		indexBuffer.destroy(renderer.openGlContext);

		loaded = false;
	}
}

Bool GPUMesh::isLoaded() const
{
	return loaded;
}

void GPUMesh::upload(Renderer& renderer, const CpuMesh& cpuMesh)
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

	vao.create(renderer.openGlContext);

	if (cpuMesh.hasPositions())
	{
		positionBuffer.create(renderer.openGlContext, cpuMesh.positions, BufferUsage::StaticDraw, "Vertex Position Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 0, positionBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	}

	if (cpuMesh.hasTextureCoordinates())
	{
		textureCoordinateBuffer.create(renderer.openGlContext, cpuMesh.textureCoordinates, BufferUsage::StaticDraw, "Texture Coordinate Vertex Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 1, textureCoordinateBuffer.getID(), 0, sizeof(Vec2), 0, 2, DataType::Float, false, 0);
	}

	if (cpuMesh.hasVertexNormals())
	{
		vertexNormalBuffer.create(renderer.openGlContext, cpuMesh.vertexNormals, BufferUsage::StaticDraw, "Vertex Normals Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 2, vertexNormalBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasTangents())
	{
		tangentBuffer.create(renderer.openGlContext, cpuMesh.tangents, BufferUsage::StaticDraw, "Vertex Tangent Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 3, tangentBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasBiTangents())
	{
		biTangentBuffer.create(renderer.openGlContext, cpuMesh.biTangents, BufferUsage::StaticDraw, "Vertex Bitangent Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 4, biTangentBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	if (cpuMesh.hasVertexColors())
	{
		vertexColorBuffer.create(renderer.openGlContext, cpuMesh.vertexColors, BufferUsage::StaticDraw, "Vertex Color Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 5, vertexColorBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, false, 0);
	}

	if (cpuMesh.hasFaceNormals())
	{
		faceNormalBuffer.create(renderer.openGlContext, cpuMesh.faceNormals, BufferUsage::StaticDraw, "Face Normals Buffer");
		vao.bindVertexBuffer(renderer.openGlContext, 6, faceNormalBuffer.getID(), 0, sizeof(Vec3), 0, 3, DataType::Float, true, 0);
	}

	const Vector<Matrix> transforms = {Matrix(1)};
	transformBuffer.create(renderer.openGlContext, transforms, BufferUsage::DynamicDraw, "Transform Buffer");

	vao.bindVertexBuffer(renderer.openGlContext, 7, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 0);
	vao.bindVertexBuffer(renderer.openGlContext, 8, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 16);
	vao.bindVertexBuffer(renderer.openGlContext, 9, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 32);
	vao.bindVertexBuffer(renderer.openGlContext, 10, transformBuffer.getID(), 0, sizeof(Matrix), 1, 4, DataType::Float, false, 48);

	indexBuffer.create(renderer.openGlContext, cpuMesh.indices, BufferUsage::StaticDraw, "Index Buffer");
	vao.bindIndexBuffer(renderer.openGlContext, indexBuffer.getID());

	loaded = true;
}

void GPUMesh::render(Renderer& renderer) const
{
	if (loaded)
	{
		renderer.uniforms().setInstanced(false);
		renderer.uniforms().upload(renderer.openGlContext);
		vao.bind(renderer.openGlContext);
		vao.render(renderer.openGlContext, primitiveMode, indexBuffer.indexCount, nullptr);
	}
}

void GPUMesh::updateVertexNormalBuffer(Renderer& renderer, const Vec3Vector& normals)
{
	if (loaded) vertexNormalBuffer.update(renderer.openGlContext, normals);
}

void GPUMesh::updateFaceNormalBuffer(Renderer& renderer, const Vec3Vector& normals)
{
	if (loaded) faceNormalBuffer.update(renderer.openGlContext, normals);
}

void GPUMesh::updateIndexBuffer(Renderer& renderer, const Vector<unsigned int>& indices)
{
	if (loaded) indexBuffer.update(renderer.openGlContext, indices);
}

void GPUMesh::updateTangentBuffer(Renderer& renderer, const Vec3Vector& tangents)
{
	if (loaded) tangentBuffer.update(renderer.openGlContext, tangents);
}

void GPUMesh::updatePositionBuffer(Renderer& renderer, const Vec3Vector& positions)
{
	if (loaded) positionBuffer.update(renderer.openGlContext, positions);
}

void GPUMesh::updateBiTangentBuffer(Renderer& renderer, const Vec3Vector& biTangents)
{
	if (loaded) biTangentBuffer.update(renderer.openGlContext, biTangents);
}

void GPUMesh::updateVertexColorBuffer(Renderer& renderer, const Vec3Vector& vertexColors)
{
	if (loaded) vertexColorBuffer.update(renderer.openGlContext, vertexColors);
}

void GPUMesh::updateTextureCoordinateBuffer(Renderer& renderer, const Vec2Vector& textureCoordinates)
{
	if (loaded) textureCoordinateBuffer.update(renderer.openGlContext, textureCoordinates);
}

void GPUMesh::renderInstances(Renderer& renderer, const Vector<Matrix>& transforms)
{
	if (loaded)
	{
		transformBuffer.update(renderer.openGlContext, transforms);
		renderer.uniforms().setInstanced(true);
		renderer.uniforms().upload(renderer.openGlContext);
		vao.bind(renderer.openGlContext);
		vao.renderInstanced(renderer.openGlContext, primitiveMode, indexBuffer.indexCount, nullptr, toIntSafe(transforms.size()));
	}
}
