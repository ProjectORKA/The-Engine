#pragma once

#include "TiledMath.hpp"

namespace External
{
	#include "GLFW/glfw3.h"
}

using ApiWindow  = External::GLFWwindow*;
using ApiMonitor = External::GLFWmonitor*;

void           apiWindowRestore(ApiWindow apiWindow);
void           apiMaximizeWindow(ApiWindow apiWindow);
void           apiWindowDecorate(ApiWindow apiWindow);
void           apiMinimizeWindow(ApiWindow apiWindow);
void           apiWindowUndecorate(ApiWindow apiWindow);
Bool           apiWindowIsCapturing(ApiWindow apiWindow);
Bool           apiWindowIsDecorated(ApiWindow apiWindow);
void           apiWindowSwapBuffers(ApiWindow apiWindow);
void           apiWindowEnableCursor(ApiWindow apiWindow);
Bool           apiWindowIsFullscreen(ApiWindow apiWindow);
void           apiWindowDisableCursor(ApiWindow apiWindow);
TiledRectangle apiWindowGetWorkableArea(ApiWindow apiWindow);
Bool           apiWindowCursorIsCaptured(ApiWindow apiWindow);
DVec2          apiWindowGetCursorPosition(ApiWindow apiWindow);
Area           apiWindowGetFramebufferSize(ApiWindow apiWindow);
Area           apiWindowGetWindowFrameSize(ApiWindow apiWindow);
Int            apiWindowGetFramebufferWidth(ApiWindow apiWindow);
Int            apiWindowGetFramebufferHeight(ApiWindow apiWindow);
Bool           apiWindowKeyIsPressed(ApiWindow apiWindow, Int key);
void           apiWindowSetExclusiveFullscreen(ApiWindow apiWindow);
void           apiWindowSetVisibility(ApiWindow apiWindow, Bool visible);
void           apiWindowResize(ApiWindow apiWindow, Int width, Int height);
void           whenWindowApiThrowsError(Int error, const Char* description);
void           apiSetWindowShouldClose(ApiWindow apiWindow, Bool shouldClose);
void           apiWindowSetCursorPosition(ApiWindow apiWindow, DVec2 position);
ApiWindow      apiCreateWindow(Area size, const Char* title, ApiWindow shared);
void           apiWindowSetWindowedMode(ApiWindow apiWindow, TiledRectangle monitorArea);
