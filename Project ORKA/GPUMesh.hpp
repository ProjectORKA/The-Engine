#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "IndexBufferObject.hpp"

struct Uniforms;
struct Renderer;

struct GPUMesh
{
	[[nodiscard]] Bool isLoaded() const;

	void unload(Renderer& renderer);
	void render(Renderer& renderer) const;
	void upload(Renderer& renderer, const CpuMesh& cpuMesh);
	void renderInstances(Renderer& renderer, const Vector<Matrix>& transforms);

	// update buffers manually
	void updateIndexBuffer(Renderer& renderer, const Vector<UInt>& indices);
	void updateTangentBuffer(Renderer& renderer, const Vec3Vector& tangents);
	void updateFaceNormalBuffer(Renderer& renderer, const Vec3Vector& normals);
	void updatePositionBuffer(Renderer& renderer, const Vec3Vector& positions);
	void updateVertexNormalBuffer(Renderer& renderer, const Vec3Vector& normals);
	void updateBiTangentBuffer(Renderer& renderer, const Vec3Vector& biTangents);
	void updateVertexColorBuffer(Renderer& renderer, const Vec3Vector& vertexColors);
	void updateTextureCoordinateBuffer(Renderer& renderer, const Vec2Vector& textureCoordinates);

private:
	OpenGLVertexArrayObject vao;
	IndexBuffer             indexBuffer;
	OpenGLVertexBuffer      vertexNormalBuffer;
	OpenGLVertexBuffer      faceNormalBuffer;
	OpenGLVertexBuffer      tangentBuffer;
	Bool                    loaded = false;
	OpenGLVertexBuffer      positionBuffer;
	OpenGLVertexBuffer      biTangentBuffer;
	OpenGLVertexBuffer      transformBuffer;
	OpenGLVertexBuffer      vertexColorBuffer;
	OpenGLVertexBuffer      textureCoordinateBuffer;
	PrimitiveMode           primitiveMode = PrimitiveMode::Triangles;
};
