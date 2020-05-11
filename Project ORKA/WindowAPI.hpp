#pragma once

#define WINDOW_API_GLFW
/*
#ifdef WINDOW_API_GLFW
#endif // WINDOW_API_GLFW
*/


#ifdef WINDOW_API_GLFW
#include "GLFW/glfw3.h"
#endif // WINDOW_API_GLFW

#ifdef WINDOW_API_GLFW
using APIWindow = GLFWwindow*;
#endif // WINDOW_API_GLFW