#pragma once

#include "Uniforms.hpp"
#include "Shader.hpp"
#include "AutoConstruct.hpp"

struct ShaderProgram {//: public AutoConstruct{

	//Uniforms* uniformsPtr = nullptr;

	Bool isLoaded = false;

	Index programID = 0;

	Index uniformBlockID = 0;

	void select();
	void rebuild() {
		//[TODO]
	};
	void destroy();
	void create(Name name, Uniforms& uniforms);
	void create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms);
};