#pragma once

#include "Transform.hpp"
#include "CpuMesh.hpp"

struct SceneNode {
	String name;
	Transform transform;
	Vector<SceneNode> children;
	Vector<Index> meshes;
};

struct Scene2 {
	SceneNode root;
	Vector<SceneNode> nodes;
	Vector<CpuMesh> meshes;

	void importFBX(Path path, ResourceManager& resourceManager);
};
