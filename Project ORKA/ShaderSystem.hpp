
#pragma once

#include "ShaderProgram.hpp"
#include "GPUBuffer.hpp"
#include "Engine.hpp"

struct ShaderSystem {
	Uniforms uniforms;
	Map<Name, Index> shaderNames;
	Index currentShaderProgramID = 0;
	Vector<ShaderProgram> shaderPrograms;

	void destroy();
	void rebuild();
	void loadDefaultShader();
	Index use(Index shaderID);
	void create(Engine& engine);
	void add(Engine engine, Name name);
	Index use(Engine engine, Name name);
	ShaderProgram& currentShaderProgram();
	Index getShaderID(Engine& engine, Name name);
	void add(Shader& vertexShader, Shader& fragmentShader, Name name);
};