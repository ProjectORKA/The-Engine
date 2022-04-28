#pragma once

#include "FileSystem.hpp"
#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Shader {
	Index shaderID;
	Bool isLoaded = false;

	void destroy();
	void create(Path path, String uniformBlock);
	void loadShaderCode(ShaderType shaderType, String shaderCode);
};