#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "IndexBufferObject.hpp"

struct Uniforms;

struct GpuMesh
{
	[[nodiscard]] Bool isLoaded() const;

	void unload();
	void upload(const CpuMesh& cpuMesh);
	void render(Uniforms& uniforms) const;
	void renderInstances(Uniforms& uniforms, const Vector<Matrix>& transforms) const;

	// update buffers manually
	void updateIndexBuffer(const Vector<UInt>& indices);
	void updateNormalBuffer(const Vec3Vector& normals) const;
	void updateTangentBuffer(const Vec3Vector& tangents) const;
	void updatePositionBuffer(const Vec3Vector& positions) const;
	void updateBiTangentBuffer(const Vec3Vector& biTangents) const;
	void updateVertexColorBuffer(const Vec3Vector& vertexColors) const;
	void updateTextureCoordinateBuffer(const Vec2Vector& textureCoordinates) const;

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
