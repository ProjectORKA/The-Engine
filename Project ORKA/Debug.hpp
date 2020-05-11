#pragma once

#include "GraphicsAPI.hpp"
#include "Basics.hpp"


#define DEBUG

//functions
void beep();
void pause();
void debugPrint(Vec2& t);
void debugPrint(Vec3 & t);
void debugPrint(Matrix & t);

//templates
template<typename T>
void debugPrint(T t) {
#ifdef DEBUG
	std::cout << t << "\n";
#endif // DEBUG
}

#ifdef GRAPHICS_API_OPENGL
void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif // GRAPHICS_API_OPENGL

void pollGraphicsAPIError();