#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "IndexBufferObject.hpp"

struct Uniforms;

struct GPUMesh
{
	[[nodiscard]] Bool isLoaded() const;

	void unload();
	void upload(const CPUMesh& cpuMesh);
	void render(Uniforms& uniforms) const;
	void renderInstances(Uniforms& uniforms, const Vector<Matrix>& transforms) const;

	// update buffers manually
	void updateIndexBuffer(const Vector<UInt>& indices);
	void updateNormalBuffer(const Vector<Vec3>& normals) const;
	void updateTangentBuffer(const Vector<Vec3>& tangents) const;
	void updatePositionBuffer(const Vector<Vec3>& positions) const;
	void updateBiTangentBuffer(const Vector<Vec3>& biTangents) const;
	void updateVertexColorBuffer(const Vector<Vec3>& vertexColors) const;
	void updateTextureCoordinateBuffer(const Vector<Vec2>& textureCoordinates) const;

private:
	OpenGLVertexArrayObject vao;
	IndexBuffer             indexBuffer;
	OpenGLVertexBuffer      normalBuffer;
	OpenGLVertexBuffer      tangentBuffer;
	Bool                    loaded = false;
	OpenGLVertexBuffer      positionBuffer;
	OpenGLVertexBuffer      biTangentBuffer;
	OpenGLVertexBuffer      transformBuffer;
	OpenGLVertexBuffer      vertexColorBuffer;
	OpenGLVertexBuffer      textureCoordinateBuffer;
	PrimitiveMode           primitiveMode = PrimitiveMode::Triangles;
};
