
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Shader {
	Index shaderID;
	Bool isValid = false;

	void load(ShaderType shaderType, Name name);
	void load(ShaderType shaderType, Path path);
	void loadShaderCode(ShaderType shaderType, String shaderCode);
	void unload();
};