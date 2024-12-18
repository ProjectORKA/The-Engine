#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Array2D.hpp"
#include "GraphicsAPI.hpp"

struct Renderer;
struct ResourceManager;

//enum class VertexDataLocation
//{
//	Position           = 0,
//	Normals            = 1,
//	TextureCoordinates = 2,
//	Indices            = 16,
//};

enum class MeshDataFlags : UShort
{
	Positions     = 1,
	TextureCoords = 2,
	Normals       = 4,
	Tangents      = 8,
	BiTangents    = 16,
	VertexColor   = 32,
};

struct CpuMesh
{
	Name          name          = "empty";
	BufferUsage   drawMode      = BufferUsage::StaticDraw;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	Vec3Vector  positions;			// 0
	Vec2Vector  textureCoordinates;	// 1
	Vec3Vector  normals;				// 2
	Vec3Vector  tangents;				// 3
	Vec3Vector  biTangents;			// 4
	Vec3Vector  vertexColors;			// 5
	Vector<Index> indices;

	[[nodiscard]] Bool isLoaded() const;
	[[nodiscard]] Bool hasNormals() const;
	[[nodiscard]] Bool hasTangents() const;
	[[nodiscard]] Bool hasPositions() const;
	[[nodiscard]] Bool hasBiTangents() const;
	[[nodiscard]] Bool hasVertexColors() const;
	[[nodiscard]] Bool hasTextureCoordinates() const;

	CpuMesh();
	explicit CpuMesh(const Graph& graph);

	void clearGeometry();
	void removeDoubles();
	void checkIntegrity();
	void move(Vec3 moveVector);
	void calculateSmoothNormals();
	void merge(const CpuMesh& source);
	void render(Renderer& renderer) const;
	void load(Name name);
	void saveMeshFile();
	void meshFromHeightMap(Array2D<Float>& heightMap, UInt size);

private:
	Bool   loaded    = false;
	UShort dataFlags = 0;

	void calculateSmoothNormalsForTriangleMesh();
	void calculateSmoothNormalsForTriangleStrip();
};

struct MeshHeader1
{
	UInt          version       = 1;
	Name          meshName      = "";
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	UInt          vertexCount   = 0;
	UInt          uvCount       = 0;
	UInt          normalCount   = 0;
	UInt          indexCount    = 0;
};

struct MeshHeader2
{
	UInt          version       = 2;
	Name          meshName      = "";
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	SizeT         vertexCount   = 0;
	SizeT         uvCount       = 0;
	SizeT         normalCount   = 0;
	SizeT         colorCount    = 0;
	SizeT         indexCount    = 0;
};
