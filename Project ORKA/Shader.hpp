#pragma once

#include "FileSystem.hpp"
#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Shader {
	Index shaderID;
	Bool isLoaded = false;

	void destroy();
	void loadShaderCode(ShaderType shaderType, String shaderCode);
	void create(ShaderType shaderType, Name name, String uniformBlock);
	void create(ShaderType shaderType, Path path, String uniformBlock);
};