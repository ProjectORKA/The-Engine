#pragma once

#include "ShaderProgram.hpp"

struct ShaderSystem {
	Uniforms uniforms;
	Map<Name, Index> shaderNames;
	Index currentShaderProgramID = 0;
	Vector<ShaderProgram> shaderPrograms;

	void create();
	void destroy();
	void add(Name name);
	void use(Name name);
	void loadDefaultShader();
	void use(Index shaderID);
	Index getShaderID(Name name);
	ShaderProgram& currentShaderProgram();
	void add(Shader& vertexShader, Shader& fragmentShader, Name name);
};