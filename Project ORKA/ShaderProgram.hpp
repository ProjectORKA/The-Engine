#pragma once

#include "GraphicsAPI.hpp"

struct ShaderProgram
{
	void               destroy();
	void               use() const;
	[[nodiscard]] Bool isLoaded() const;
	void               create(const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode);

private:
	Bool                loaded = false;
	OpenGlShaderProgram openglShaderProgram;
};
