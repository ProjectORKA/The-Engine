#pragma once

#include "Transform.hpp"
#include "CPUMesh.hpp"

struct SceneNode
{
	String            name;
	Vector<Index>     meshes;
	Vector<SceneNode> children;
	Transform         transform;
};

struct Scene2
{
	SceneNode         root;
	Vector<SceneNode> nodes;
	Vector<CpuMesh>   meshes;

	void importFBX(Path path);
};
