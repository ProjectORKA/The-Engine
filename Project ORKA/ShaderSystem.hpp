
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
	void add(ResourceManager & resourceManager, Name name);
	Index use(ResourceManager& resourceManager, Name name);
	Index getShaderID(ResourceManager& resourceManager, Name name);
	void add(Shader& vertexShader, Shader& fragmentShader, Name name);
};