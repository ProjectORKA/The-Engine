#pragma once

#include "Debug.hpp"
#include "PrimitiveModes.hpp"
#include "CPUMesh.hpp"

enum BufferUsage {
	StaticBufferUsage = GL_STATIC_DRAW,
	StreamBufferUsage = GL_STREAM_DRAW
};

struct VertexBufferObject {
	Index bufferID = 0;
	Index location = 0;
	Byte components = 3;
	UInt byteSize = 0;

	void create(UInt location, float* data, UInt byteSize, UInt usage, Index components);
	void unload();
};

struct IndexBufferObject {
	Index bufferID;
	UInt indexCount;
	void create(Index* data, UInt byteSize, UInt usage);
	void unload();
};

struct VertexArrayObject {
	Index arrayObjectID;
	Vector<VertexBufferObject> buffers;
	IndexBufferObject indexBuffer;

	void create(CPUMesh& mesh);
	void add(Index location, float* data, UInt byteSize, UInt usage, Index components);
	void render();
	void unload();

};

struct GPUMesh {
	Bool loaded = false;
	PrimitiveMode primitiveMode = Triangles;

	VertexArrayObject vao;

	void upload(CPUMesh& cpuMesh);
	void render();
	void unload();
};