#pragma once

#include "BasicsModule.hpp"

#include "Debug.hpp"
#include "PrimitiveModes.hpp"
//assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct CPUMesh {
	Name name = "empty";
	PrimitiveMode primitiveMode = Triangles;
	Vector<Vec3> vertices;
	Vector<Vec2> uvs;
	Vector<Index> indices;
	Bool readyForUpload = false;
	
	void autoLoadFromFile(Name name);
	void loadFBX(Path path);
	void loadMeshFile(Path path);
	void saveMeshFile();
};

struct MeshHeaderV1 {
	const unsigned int version = 1;
	Name meshName = "";
	PrimitiveMode primitiveMode = Triangles;
	unsigned int vertexCount = 0;
	unsigned int uvCount = 0;
	unsigned int indexCount = 0;
};