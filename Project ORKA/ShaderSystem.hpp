#pragma once

#include "Debug.hpp"
#include "Shader.hpp"

struct ShaderSystem;

struct ShaderSystem {
	Map<Name, Index> shaderNames;
	Vector<ShaderProgram> shaderPrograms;
	Index currentShaderProgramID = 0;

	//temporary Shaders
	Shader vertexShader;
	Shader fragmentShader;

	void create();
	void destroy();
	void use(Name name);
	void loadDefaultShader();
	void use(Index shaderID);
	void loadShaderProgram(Name name);
	ShaderProgram& currentShaderProgram();
	void add(Shader& vertexShader, Shader& fragmentShader, Name name);
	
	//uniforms
	Uniforms uniforms;
	//void setBool(Name name, Bool value);
	//void setInt(Name name, Int value);
	//void setFloat(Name name, Float value);
	//void setVec3(Name name, Vec3 value);
	//void setVec4(Name name, Vec4 value);
	//void setMatrix(Name name, Matrix value);
};