#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
//assimp 
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct CPUMesh {
	Name name = "empty";
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Vec3> normals;
	Vector<Index> indices;
	MeshDrawMode drawMode = MeshDrawMode::staticMode;
	
	Bool readyForUpload = false;
	
	CPUMesh();
	CPUMesh(Name name);
	~CPUMesh(){};

	void saveMeshFile();
	void load(Name name);
	void loadFBX(Path path);
	void loadMeshFile(Path path);
	void calculateSmoothNormals();
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
	PrimitiveMode primitiveMode = PrimitiveMode::Triangles;
	unsigned int vertexCount = 0;
	unsigned int uvCount = 0;
	unsigned int normalCount = 0;
	unsigned int indexCount = 0;
};