#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "PrimitiveModes.hpp"
#include "TerrainSystem.hpp"
//assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct CPUMesh {
	Name name = "empty";
	PrimitiveMode primitiveMode = Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Vec3> normals;
	Vector<Index> indices;
	Bool readyForUpload = false;
	
	void saveMeshFile();
	void loadFBX(Path path);
	void loadMeshFile(Path path);
	void autoLoadFromFile(Name name);
	void generate2DTerrainMesh(Terrain& terrain);
};

//struct MeshHeaderV1 {
//	const unsigned int version = 1;
//	Name meshName = "";
//	PrimitiveMode primitiveMode = Triangles;
//	unsigned int vertexCount = 0;
//	unsigned int uvCount = 0;
//	unsigned int indexCount = 0;
//};

struct MeshHeaderV2 {
	const unsigned int version = 2;
	Name meshName = "";
	PrimitiveMode primitiveMode = Triangles;
	unsigned int vertexCount = 0;
	unsigned int uvCount = 0;
	unsigned int normalCount = 0;
	unsigned int indexCount = 0;
};