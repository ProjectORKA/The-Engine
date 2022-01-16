#pragma once

#include "Uniforms.hpp"
#include "Shader.hpp"
#include "AutoConstruct.hpp"

struct ShaderProgram {
	Bool isLoaded = false;
	Index programID = 0;
	Index uniformBlockID = 0;

	void select();
	void destroy();
	void create(Name name, Uniforms& uniforms);
	void create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms);
};