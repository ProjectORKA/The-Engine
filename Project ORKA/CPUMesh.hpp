#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Array2D.hpp"
#include "GraphicsAPI.hpp"

struct Renderer;
struct ResourceManager;

enum class MeshDataFlags : UShort
{
	Positions     = 1,	// 1st bit
	TextureCoords = 2,	// 2nd bit
	VertexNormals = 4,	// 3rd bit
	Tangents      = 8,	// 4th bit
	BiTangents    = 16,	// 5th bit
	VertexColor   = 32,	// 6th bit
	FaceNormals   = 64	// 7th bit
};

struct CpuMesh
{
	Name          name          = "empty";
	BufferUsage   drawMode      = BufferUsage::StaticDraw;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	Vec3Vector    positions;          // 0
	Vec2Vector    textureCoordinates; // 1
	Vec3Vector    vertexNormals;      // 2
	Vec3Vector    tangents;           // 3
	Vec3Vector    biTangents;         // 4
	Vec3Vector    vertexColors;       // 5
	Vec3Vector    faceNormals;        // 6
	Vector<Index> indices;

	[[nodiscard]] Bool isLoaded() const;
	[[nodiscard]] Bool hasTangents() const;
	[[nodiscard]] Bool hasPositions() const;
	[[nodiscard]] Bool hasBiTangents() const;
	[[nodiscard]] Bool hasFaceNormals() const;
	[[nodiscard]] Bool hasVertexColors() const;
	[[nodiscard]] Bool hasVertexNormals() const;
	[[nodiscard]] Bool hasTextureCoordinates() const;

	CpuMesh();
	explicit CpuMesh(const Graph& graph);

	void saveMeshFile();
	void clearGeometry();
	void removeDoubles();
	void load(Name name);
	void checkIntegrity();
	void move(Vec3 moveVector);
	void calculateFlatNormals();
	void calculateSmoothNormals();
	void merge(const CpuMesh& source);
	void render(Renderer& renderer) const;
	void meshFromHeightMap(Array2D<Float>& heightMap, UInt size);

private:
	Bool   loaded    = false;
	UShort dataFlags = 0;

	void calculateFlatNormalsForTriangleMesh();
	void calculateSmoothNormalsForTriangleMesh();
	void calculateSmoothNormalsForTriangleStrip();
};

//struct MeshHeader1
//{
//	UInt          version       = 1;
//	Name          meshName      = "";
//	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
//	UInt          vertexCount   = 0;
//	UInt          uvCount       = 0;
//	UInt          normalCount   = 0;
//	UInt          indexCount    = 0;
//};
//
//struct MeshHeader2
//{
//	UInt          version       = 2;
//	Name          meshName      = "";
//	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
//	SizeT         vertexCount   = 0;
//	SizeT         uvCount       = 0;
//	SizeT         normalCount   = 0;
//	SizeT         colorCount    = 0;
//	SizeT         indexCount    = 0;
//};

struct MeshHeader3
{
	UInt          version           = 3;
	Name          meshName          = "";
	PrimitiveMode primitiveMode     = PrimitiveMode::Triangles;
	SizeT         vertexCount       = 0;
	SizeT         uvCount           = 0;
	SizeT         vertexNormalCount = 0;
	SizeT         faceNormalCount   = 0;
	SizeT         colorCount        = 0;
	SizeT         indexCount        = 0;
};
