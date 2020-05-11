#pragma once

#include "GraphicsAPI.hpp"

using PrimitiveMode = unsigned short;
enum PrimitiveModes
{
#ifdef GRAPHICS_API_OPENGL
	Triangles = GL_TRIANGLES,
	Points = GL_POINTS,
	Lines = GL_LINES
#endif // GRAPHICS_API_OPENGL
};