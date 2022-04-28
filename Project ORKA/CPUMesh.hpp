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
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Vec3> normals;
	Vector<Index> materialID;

	Vector<Index> indices;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;
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

struct MeshHeader {
	UInt version = 1;
	Name meshName = "";
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	UInt vertexCount = 0;
	UInt uvCount = 0;
	UInt normalCount = 0;
	UInt indexCount = 0;
};