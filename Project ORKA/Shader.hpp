#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct Shader {
	Index shaderId = -1;
	Bool isLoaded = false;

	void destroy();
	void create(const Path& path, const String& uniformBlock);
	void loadShaderCode(ShaderType shaderType, const String& shaderCode);
};
