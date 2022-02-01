#pragma once

#include "Debug.hpp"
#include "TiledMath.hpp"
#include "KeyMap.hpp"
#include "GLFW/glfw3.h"

using APIWindow = GLFWwindow*;
using APIMonitor = GLFWmonitor*;

void apiWindowRestore(APIWindow apiWindow);
void apiMaximizeWindow(APIWindow apiWindow);
void apiWindowDecorate(APIWindow apiWindow);
void apiMinimizeWindow(APIWindow apiWindow);
void apiWindowUndecorate(APIWindow apiWindow);
Bool apiWindowShouldClose(APIWindow apiWindow);
Bool apiWindowIsCapturing(APIWindow apiWindow);
Bool apiWindowIsDecorated(APIWindow apiWindow);
void apiWindowSwapBuffers(APIWindow apiWindow);
void apiWindowEnableCursor(APIWindow apiWindow);
Bool apiWindowIsFullscreen(APIWindow apiWindow);
void apiWindowDisableCursor(APIWindow apiWindow);
Bool apiWindowCursorIsCaptured(APIWindow apiWindow);
Int apiWindowGetFramebufferWidth(APIWindow apiWindow);
DVec2 apiWindowGetCursorPosition(APIWindow apiWindow);
Area apiWindowGetFramebufferSize(APIWindow apiWindow);
Area apiWindowGetWindowFrameSize(APIWindow apiWindow);
Int apiWindowGetFramebufferHeight(APIWindow apiWindow);
APIWindow apiCreateWindow(Area size, const Char* title);
Bool apiWindowKeyIsPressed(APIWindow apiWindow, Key key);
void apiWindowSetExclusiveFullscreen(APIWindow apiWindow);
TiledRectangle apiWindowGetWorkableArea(APIWindow apiWindow);
void apiWindowSetVisibility(APIWindow apiWindow, Bool visible);
void apiWindowResize(APIWindow apiWindow, Int width, Int height);
void whenWindowAPIThrowsError(Int error, const Char* description);
void apiWindowSetCursorPosition(APIWindow apiWindow, Vec2 position);
void apiWindowSetWindowedMode(APIWindow apiWindow, TiledRectangle monitorArea);