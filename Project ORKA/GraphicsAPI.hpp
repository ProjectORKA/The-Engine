#pragma once

#define GRAPHICS_API_OPENGL

#ifdef GRAPHICS_API_OPENGL
//opengl/glew
#define GLAPI
#define GLEW_STATIC
#include "GL/glew.h"
#endif // GRAPHICS_API_OPENGL

