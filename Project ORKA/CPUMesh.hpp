#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Debug.hpp"
#include "Array2D.hpp"
#include "FileSystem.hpp"
#include "GraphicsAPI.hpp"

struct Renderer;

enum class VertexDataLocation {
	Position = 0,
	Normals = 1,
	TextureCoordinates = 2,
	Indices = 16,
};

struct CPUMesh {
	Name name = "empty";
	Int primitiveMode = PrimitiveMode::Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Vec3> normals;
	Vector<Vec3> colors;

	Vector<Index> indices;
	Int drawMode = MeshDrawMode::staticMode;
	Bool readyForUpload = false;

	CPUMesh() {};
	~CPUMesh() {};
	CPUMesh(Name name);
	CPUMesh(Graph& graph);

	void saveMeshFile();
	void removeDoubles();
	void load(Name name);
	void checkIntegrity();
	void move(Vec3 moveVector);
	void merge(CPUMesh source);
	void calculateSmoothNormals();
	void render(Renderer& renderer);
	void meshFromHeightmap(Array2D<Float>& heightmap, UInt size);
};

struct MeshHeader1 {
	UInt version = 1;
	Name meshName = "";
	Int primitiveMode = PrimitiveMode::Triangles;
	UInt vertexCount = 0;
	UInt uvCount = 0;
	UInt normalCount = 0;
	UInt indexCount = 0;
};

struct MeshHeader2 {
	UInt version = 2;
	Name meshName = "";
	Int primitiveMode = PrimitiveMode::Triangles;
	UInt vertexCount = 0;
	UInt uvCount = 0;
	UInt normalCount = 0;
	UInt colorCount = 0;
	UInt indexCount = 0;
};