
#pragma once

#include "Basics.hpp"
#include "Uniforms.hpp"
#include "GraphicsAPI.hpp"
#include "FileSystem.hpp"

enum ShaderType {
	VertexShaderType = GL_VERTEX_SHADER,
	TessellationControlShaderType = GL_TESS_CONTROL_SHADER,
	TessellationEvaluationShaderType = GL_TESS_EVALUATION_SHADER,
	GeometryShaderType = GL_GEOMETRY_SHADER,
	FragmentShaderType = GL_FRAGMENT_SHADER,
	ComputeShaderType = GL_COMPUTE_SHADER
};

struct Shader {
	Index shaderID;
	Bool isValid = false;

	void loadFromFile(Index shaderType, Path path);
	void load(Index shaderType, String shaderCode);
	void unload();
};
struct ShaderProgram {
	
	Uniforms* uniformsPtr = nullptr;
	
	Bool isValid = false;

	Index programID = 0;

	Vector<Uniform> usedUniforms;

	void use();
	void unload();
	void updateUniforms();
	void create(Shader& vertexShader, Shader& fragmentShader, Uniforms& uniforms);
};