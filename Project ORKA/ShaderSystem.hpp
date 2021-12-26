#pragma once

#include "ShaderProgram.hpp"

struct ShaderSystem {
	Uniforms uniforms;
	Map<Name, Index> shaderNames;
	Index currentShaderProgramID = 0;
	Vector<ShaderProgram> shaderPrograms;

	void create();
	void destroy();
	void rebuild();
	void add(Name name);
	Index use(Name name);
	void loadDefaultShader();
	Index use(Index shaderID);
	Index getShaderID(Name name);
	ShaderProgram& currentShaderProgram();
	void add(Shader& vertexShader, Shader& fragmentShader, Name name);
};