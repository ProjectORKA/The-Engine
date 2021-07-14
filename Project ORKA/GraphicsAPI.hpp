#pragma once

//opengl/glew
#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#include "Debug.hpp"

enum class ShaderType {
	vertex = GL_VERTEX_SHADER,
	compute = GL_COMPUTE_SHADER,
	geometry = GL_GEOMETRY_SHADER,
	fragment = GL_FRAGMENT_SHADER,
	tessellationControl = GL_TESS_CONTROL_SHADER,
	tessellationEvaluation = GL_TESS_EVALUATION_SHADER
};
enum class MeshDrawMode {
	staticMode = GL_STATIC_DRAW,
	dynamicMode = GL_DYNAMIC_DRAW,
	streamMode = GL_STREAM_DRAW,
};
enum class PrimitiveMode
{
	Lines = GL_LINES,
	Points = GL_POINTS,
	Triangles = GL_TRIANGLES,
	TriangleStrip = GL_TRIANGLE_STRIP
};

void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);