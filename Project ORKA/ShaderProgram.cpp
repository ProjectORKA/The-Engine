#include "ShaderProgram.hpp"

void ShaderProgram::destroy()
{
	if(isLoaded())
	{
		openglShaderProgram.destroy();
	}
	else
	{
		logError("ShaderProgram already unloaded!");
	}
}

void ShaderProgram::use() const
{
	if(loaded)
	{
		openglShaderProgram.use();
	}
	else
	{
		logError("ShaderProgram not loaded!");
	}
}

Bool ShaderProgram::isLoaded() const
{
	return loaded;
}

void ShaderProgram::create(const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode)
{
	if(loaded)
	{
		logError("Shader program already loaded!");
		return;
	}

	loaded = openglShaderProgram.create(toString(name), vertexShaderCode, fragmentShaderCode);
}
