#pragma once

#include "GraphicsAPI.hpp"

struct ShaderProgram
{
	[[nodiscard]] Bool isLoaded() const;
	void               destroy(OpenGlContext& openGlContext);
	void               use(OpenGlContext& openGlContext) const;
	void               create(OpenGlContext& openGlContext, const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode);
	void               createCompute(OpenGlContext& openGlContext, const Name& name, const String& computeShaderCode);

private:
	Bool                loaded = false;
	OpenGLShaderProgram openglShaderProgram;
};
