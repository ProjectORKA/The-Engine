#pragma once

#include "ShaderProgram.hpp"
#include "Uniforms.hpp"

struct Renderer;

struct ShaderSystem
{
	Uniforms              uniforms;
	Map<Name, Index>      shaderNames;
	Vector<ShaderProgram> shaderPrograms;
	UInt                  currentShaderProgramId = 0;

	ShaderProgram& currentShaderProgram();
	void           create(Renderer& renderer);
	void           destroy(Renderer& renderer);
	void           rebuild(Renderer& renderer);
	void           loadDefaultShader(Renderer& renderer);
	Index          use(Renderer& renderer, Index shaderId);
	void           add(Renderer& renderer, const Name& name);
	Index          use(Renderer& renderer, const Name& name);
	Index          getShaderId(Renderer& renderer, const Name& name);
	void           add(Renderer& renderer, const Name& name, const String& vertexShaderCode, const String& fragmentShaderCode);
};
