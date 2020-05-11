#pragma once

#include "Uniforms.hpp"
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Files.hpp"

struct ShaderSystem;

enum ShaderType {
#ifdef GRAPHICS_API_OPENGL
	VertexShaderType = GL_VERTEX_SHADER,
	TessellationControlShaderType = GL_TESS_CONTROL_SHADER,
	TessellationEvaluationShaderType = GL_TESS_EVALUATION_SHADER,
	GeometryShaderType = GL_GEOMETRY_SHADER,
	FragmentShaderType = GL_FRAGMENT_SHADER,
	ComputeShaderType = GL_COMPUTE_SHADER
#endif // GRAPHICS_API_OPENGL
};
struct Shader {
	Index shaderID;

	void loadFromFile(Index shaderType, Path path);
	void load(Index shaderType, String shaderCode);
	void unload();
};
struct ShaderProgram {
	Bool loaded = false;

	Index programID = 0;

	Vector<String> uniformNames;
	Vector<UShort> type;
	Vector<Index> shaderUniformIDs;

	void load(Shader& vertexShader, Shader& fragmentShader);
	void use(Uniforms & uniforms);
	void unload();
};

struct ShaderSystem {
	Map<String, Index> shaderNames;
	Vector<ShaderProgram> shaderPrograms;
	Uniforms uniforms;

	//temporary Shaders
	Shader vertexShader;
	Shader fragmentShader;

	void create();
	void destroy();

	void add(Shader& vertexShader, Shader& fragmentShader, String name);
	void useShader(Index shaderID);
	void useShader(String name);
};