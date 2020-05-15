#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "PrimitiveModes.hpp"
//assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct CPUMesh {
	String name = "empty";
	PrimitiveMode primitiveMode = Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Index> indices;
	Bool readyForUpload = false;
	
	void loadFBX(String name, PrimitiveMode mode, Path path);

	void loadMeshFile(Path path);
	void saveMeshFile();
};

struct MeshHeaderV1 {
	const unsigned int version = 1;
	char meshName[100] = {0};
	PrimitiveMode primitiveMode = Triangles;
	unsigned int vertexCount = 0;
	unsigned int uvCount = 0;
	unsigned int indexCount = 0;
};