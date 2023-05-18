#pragma once

#include "Memory.hpp"
#include "Basics.hpp"
#include "Math.hpp"
#include "Debug.hpp"
#include "Array2D.hpp"
#include "FileSystem.hpp"
#include "GraphicsAPI.hpp"
#include "ResourceManager.hpp"

struct Renderer;
struct ResourceManager;

enum class VertexDataLocation {
	Position = 0,
	Normals = 1,
	TextureCoordinates = 2,
	Indices = 16,
};

enum MeshData : Short {
	Positions = 1,
	TextureCoords = 2,
	Normals = 4,
	Tangents = 8,
	BiTangents = 16,
	VertexColor = 32,
};

struct CpuMesh {
	Bool loaded = false;

	Name name = "empty";
	Short dataFlags = 0;
	Int drawMode = MeshDrawMode::staticMode;
	Int primitiveMode = PrimitiveMode::Triangles;

	Vector<Vec3> positions;
	Vector<Vec2> textureCoordinates;
	Vector<Vec3> normals;
	Vector<Vec3> tangents;
	Vector<Vec3> biTangents;
	Vector<Vec3> vertexColors;

	Vector<Index> indices;

	CpuMesh() {}
	~CpuMesh() {}
	explicit CpuMesh(const Graph& graph);

	void clearGeometry();
	void removeDoubles();
	void checkIntegrity();
	void move(Vec3 moveVector);
	void merge(const CpuMesh& source);
	void calculateSmoothNormals();
	void render(Renderer& renderer) const;
	void saveMeshFile(const ResourceManager& resourceManager);
	void load(ResourceManager& resourceManager, Name name);
	void meshFromHeightMap(Array2D<Float>& heightMap, UInt size);

private:
	void calculateSmoothNormalsForTriangleMesh();
	void calculateSmoothNormalsForTriangleStrip();
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
