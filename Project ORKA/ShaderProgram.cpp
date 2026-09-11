#include "ShaderProgram.hpp"

void ShaderProgram::destroy(OpenGlContext& openGlContext)
{
	if(isLoaded()) openglShaderProgram.destroy(openGlContext);
	else logError("ShaderProgram already unloaded!");
}

void ShaderProgram::use(OpenGlContext& openGlContext) const
{
	if(loaded) openglShaderProgram.use(openGlContext);
	else logError("ShaderProgram not loaded!");
}

Bool ShaderProgram::isLoaded() const
{
	return loaded;
}

void ShaderProgram::create(OpenGlContext& openGlContext, const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode)
{
	if(loaded)
	{
		logError("Shader program already loaded!");
		return;
	}

	loaded = openglShaderProgram.create(openGlContext, toString(name), vertexShaderCode, fragmentShaderCode);
}

void ShaderProgram::createCompute(OpenGlContext& openGlContext, const Name& name, const String& computeShaderCode)
{
	if(loaded)
	{
		logError("Shader program already loaded!");
		return;
	}

	loaded = openglShaderProgram.createCompute(openGlContext, toString(name), computeShaderCode);
}
