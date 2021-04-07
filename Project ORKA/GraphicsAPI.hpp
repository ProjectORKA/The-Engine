#pragma once

//opengl/glew
#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#include "Debug.hpp"

enum {
	StaticBufferUsage = GL_STATIC_DRAW,
	DynamicBufferUsage = GL_DYNAMIC_DRAW
};

using PrimitiveMode = unsigned short;
enum PrimitiveModes
{
	Lines = GL_LINES,
	Points = GL_POINTS,
	Triangles = GL_TRIANGLES,
	TriangleStrip = GL_TRIANGLE_STRIP
};

void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);