#pragma once


#include "FileSystem.hpp"
#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Shader {
	Index shaderID;
	Bool isLoaded = false;

	void load(ShaderType shaderType, Name name);
	void load(ShaderType shaderType, Path path);
	void loadShaderCode(ShaderType shaderType, String shaderCode);
	void unload();
};