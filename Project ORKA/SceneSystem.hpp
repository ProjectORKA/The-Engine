
#pragma once

#include "Transform.hpp"
#include "CPUMesh.hpp"

struct SceneNode {
	String name;
	Transform transform;
	std::vector<SceneNode> children;
	std::vector<Index> meshes;
};

//struct Asset {
//	String name;
//	SceneNode root;
//};

struct Scene2 {
	SceneNode root;
	std::vector<SceneNode> nodes;
	std::vector<CPUMesh> meshes;

	void importFBX(Path path, ResourceManager& resourceManager);
};