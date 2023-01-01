
#pragma once

#include "CPUMesh.hpp"

struct Scene {
	Vector<CPUMesh> meshes;
	//materials
	//textures
	//animations
	//etc
	void loadFBX(Path path, ResourceManager& resourceManager);
};