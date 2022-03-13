#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Debug.hpp"
#include "Array2D.hpp"
#include "FileSystem.hpp"
#include "GraphicsAPI.hpp"
//assimp 
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

struct Renderer;

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
	~CPUMesh();
	CPUMesh(Name name);
	CPUMesh(Graph& graph);
	CPUMesh(Name name, MeshDrawMode drawMode, PrimitiveMode primitiveMode);

	void saveMeshFile();
	void load(Name name);
	void removeDoubles();
	void checkIntegrity();
	void loadFBX(Path path);
	void move(Vec3 moveVector);
	void merge(CPUMesh source);
	void loadMeshFile(Path path);
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