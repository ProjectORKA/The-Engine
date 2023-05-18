#pragma once
#include "Basics.hpp"
#include "CpuMesh.hpp"
#include "ResourceManager.hpp"

struct Scene {
	Vector<CpuMesh> meshes;
	//materials
	//textures
	//animations
	//etc
	void loadFBX(Path path, ResourceManager& resourceManager);
};
