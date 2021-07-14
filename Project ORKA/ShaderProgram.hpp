#pragma once

#include "Uniforms.hpp"
#include "Shader.hpp"

struct ShaderProgram {

	Uniforms* uniformsPtr = nullptr;

	Bool isValid = false;

	Index programID = 0;

	Index uniformBlockID = 0;

	void select();
	void unload();
	void load(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms);
	void load(Name name, Uniforms& uniforms);
};