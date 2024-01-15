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
	void           create();
	void           add(const Name& name);
	Index          use(const Name& name);
	Index          getShaderId(const Name& name);
	void           add(const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode);
};
