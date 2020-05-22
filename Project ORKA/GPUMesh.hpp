#pragma once

#include "Debug.hpp"
#include "PrimitiveModes.hpp"
#include "CPUMesh.hpp"

enum BufferUsage {
	StaticBufferUsage = GL_STATIC_DRAW,
	StreamBufferUsage = GL_STREAM_DRAW
};

struct VertexBufferObject {

	Name name = ("vertex");
	Index bufferID;

	void create(UInt location, float* data, UInt byteSize, UInt usage, Index components);
	void unload();

};

struct IndexBufferObject {
	Index bufferID;
	void create(Index* data, UInt byteSize, UInt usage);
	void unload();
};

struct VertexArrayObject {
	Index arrayObjectID;
	Index vertexAttributeCount = 0;
	Vector<VertexBufferObject> buffers;
	IndexBufferObject indexBuffer;

	void create(CPUMesh& mesh);
	void add(float* data, UInt byteSize, UInt usage, Index components);
	void render();
	void unload();

};

struct GPUMesh {
	Bool loaded = false;
	Int indexCount = 0;
	Index vertexArrayObjectID = 0;
	Index vertexBufferID = 0;
	Index uvBufferID = 0;
	Index indexBufferID = 0;
	PrimitiveMode primitiveMode = Triangles;

	VertexArrayObject vao;

	void upload(CPUMesh & cpuMesh);
	void render();
	void unload();
};