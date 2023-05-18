#pragma once

#include "ShaderProgram.hpp"
#include "GPUBuffer.hpp"

struct ShaderSystem {
	Uniforms uniforms;
	Map<Name, Index> shaderNames;
	Index currentShaderProgramID = 0;
	Vector<ShaderProgram> shaderPrograms;

	void destroy();
	void rebuild();
	void loadDefaultShader();
	Index use(Index shaderID);
	ShaderProgram& currentShaderProgram();
	void create(ResourceManager& resourceManager);
	void add(ResourceManager& resourceManager, const Name& name);
	Index use(ResourceManager& resourceManager, const Name& name);
	Index getShaderID(ResourceManager& resourceManager, const Name& name);
	void add(const Shader& vertexShader, const Shader& fragmentShader, const Name& name);
};
