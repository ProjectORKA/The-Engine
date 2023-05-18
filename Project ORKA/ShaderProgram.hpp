#pragma once

#include "Uniforms.hpp"
#include "Shader.hpp"
#include "AutoConstruct.hpp"
#include "ResourceManager.hpp"

struct ShaderProgram {
	Bool isLoaded = false;
	Index programID = 0;
	Index uniformBlockID = 0;

	void select() const;
	void destroy() const;
	void create(ResourceManager& resourceManager, const Name& name, Uniforms& uniforms);
	void create(const Shader& vertexShader, const Shader& fragmentShader, Uniforms& uniforms);
};
