#pragma once
#include "Basics.hpp"
#include "CPUMesh.hpp"


struct Scene
{
	Vector<CPUMesh> meshes;
	// materials
	// textures
	// animations
	// etc
	void loadFBX(Path path);
};
