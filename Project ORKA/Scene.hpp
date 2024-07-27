#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"


struct Scene
{
	Vector<CpuMesh> meshes;
	// materials
	// textures
	// animations
	// etc
	void loadFBX(Path path);
};
