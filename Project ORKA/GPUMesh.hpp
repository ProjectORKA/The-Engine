#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "IndexBufferObject.hpp"

struct Uniforms;

struct GPUMesh
{
	void               unload();
	[[nodiscard]] Bool isLoaded() const;
	void               upload(const CPUMesh& cpuMesh);
	void               render(Uniforms& uniforms) const;
	void               renderInstances(Uniforms& uniforms, const Vector<Matrix> & transforms) const;
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
