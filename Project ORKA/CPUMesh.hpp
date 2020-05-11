#pragma once

#include "Basics.hpp"
#include "MeshFileInfo.hpp"
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
	
	void load(MeshFileInfo & info);
	void load(String name, PrimitiveMode mode, Path path);
};