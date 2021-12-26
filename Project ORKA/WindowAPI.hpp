#pragma once

#include "Debug.hpp"
#include "TiledMath.hpp"
#include "KeyMap.hpp"
#include "GLFW/glfw3.h"

using APIWindow = GLFWwindow*;
using APIMonitor = GLFWmonitor*;

void apiWindowRestore(APIWindow apiWindow);
void apiWindowDecorate(APIWindow apiWindow);
void apiWindowUndecorate(APIWindow apiWindow);
void apiWindowSwapBuffers(APIWindow apiWindow);
void apiWindowEnableCursor(APIWindow apiWindow);
void apiWindowDisableCursor(APIWindow apiWindow);
void apiWindowSetExclusiveFullscreen(APIWindow apiWindow);
void apiWindowSetVisibility(APIWindow apiWindow, Bool visible);
void apiWindowSetCursorPosition(APIWindow apiWindow, Vec2 position);
void apiWindowSetWindowedMode(APIWindow apiWindow, TiledRectangle monitorArea);

//get boolean
Bool apiWindowShouldClose(APIWindow apiWindow);
Bool apiWindowIsCapturing(APIWindow apiWindow);
Bool apiWindowIsDecorated(APIWindow apiWindow);
Bool apiWindowIsFullscreen(APIWindow apiWindow);
Bool apiWindowCursorIsCaptured(APIWindow apiWindow);
Bool apiWindowKeyIsPressed(APIWindow apiWindow, Key key);

//get integer
Int apiWindowGetFramebufferWidth(APIWindow apiWindow);
Int apiWindowGetFramebufferHeight(APIWindow apiWindow);

//get others
DVec2 apiWindowGetCursorPosition(APIWindow apiWindow);
Area apiWindowGetFramebufferSize(APIWindow apiWindow);
Area apiWindowGetWindowFrameSize(APIWindow apiWindow);
TiledRectangle apiWindowGetWorkableArea(APIWindow apiWindow);

void whenWindowAPIThrowsError(Int error, const char* description);