
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

struct Shader {
	Index shaderID;
	Bool isLoaded = false;

	void destroy();
	void create(Path path, String uniformBlock);
	void loadShaderCode(Int shaderType, String shaderCode);
};