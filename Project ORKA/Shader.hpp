#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

struct Shader {
	Index shaderID;
	Bool isLoaded = false;

	void destroy();
	void create(const Path& path, const String& uniformBlock);
	void loadShaderCode(Int shaderType, const String& shaderCode);
};
