#pragma once

#include "ShaderProgram.hpp"
#include "Uniforms.hpp"

struct ShaderSystem
{
	Uniforms              uniforms;
	Map<Name, Index>      shaderNames;
	Vector<ShaderProgram> shaderPrograms;
	UInt                  currentShaderProgramId = 0;

	void           destroy();
	void           rebuild();
	void           loadDefaultShader();
	Index          use(Index shaderId);
	ShaderProgram& currentShaderProgram();
	void           create(const ResourceManager& resourceManager);
	void           add(ResourceManager& resourceManager, const Name& name);
	Index          use(ResourceManager& resourceManager, const Name& name);
	Index          getShaderId(ResourceManager& resourceManager, const Name& name);
	void           add(const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode);
};
