#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Array2D.hpp"
#include "GraphicsAPI.hpp"

struct Renderer;
struct ResourceManager;

enum class VertexDataLocation
{
	Position           = 0,
	Normals            = 1,
	TextureCoordinates = 2,
	Indices            = 16,
};

enum class MeshDataFlags : UShort
{
	Positions     = 1,
	TextureCoords = 2,
	Normals       = 4,
	Tangents      = 8,
	BiTangents    = 16,
	VertexColor   = 32,
};

struct CPUMesh
{
	Name          name          = "empty";
	BufferUsage   drawMode      = BufferUsage::StaticDraw;
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;

	Vector<Vec3>  positions;			// 0
	Vector<Vec2>  textureCoordinates;	// 1
	Vector<Vec3>  normals;				// 2
	Vector<Vec3>  tangents;				// 3
	Vector<Vec3>  biTangents;			// 4
	Vector<Vec3>  vertexColors;			// 5
	Vector<Index> indices;

	[[nodiscard]] Bool isLoaded() const;
	[[nodiscard]] Bool hasNormals() const;
	[[nodiscard]] Bool hasTangents() const;
	[[nodiscard]] Bool hasPositions() const;
	[[nodiscard]] Bool hasBiTangents() const;
	[[nodiscard]] Bool hasVertexColors() const;
	[[nodiscard]] Bool hasTextureCoordinates() const;

	CPUMesh();
	explicit CPUMesh(const Graph& graph);

	void clearGeometry();
	void removeDoubles();
	void checkIntegrity();
	void move(Vec3 moveVector);
	void calculateSmoothNormals();
	void merge(const CPUMesh& source);
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
