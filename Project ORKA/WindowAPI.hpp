#pragma once

#include "TiledMath.hpp"

namespace External
{
	#include <GLFW/glfw3.h>
}

using ApiWindow  = External::GLFWwindow*;
using ApiMonitor = External::GLFWmonitor*;

enum class ApiCursorType : Int
{
	Arrow = 0,
	Hand,
	IBeam,
};

void           apiWindowRestore(ApiWindow apiWindow);
void           apiMaximizeWindow(ApiWindow apiWindow);
void           apiWindowDecorate(ApiWindow apiWindow);
void           apiMinimizeWindow(ApiWindow apiWindow);
void           apiWindowUndecorate(ApiWindow apiWindow);
Bool           apiWindowIsCapturing(ApiWindow apiWindow);
Bool           apiWindowIsDecorated(ApiWindow apiWindow);
void           apiWindowSwapBuffers(ApiWindow apiWindow);
TiledRectangle apiWindowGetWorkArea(ApiWindow apiWindow);
void           apiWindowEnableCursor(ApiWindow apiWindow);
void           apiWindowHideCursor(ApiWindow apiWindow);
Bool           apiWindowIsFullscreen(ApiWindow apiWindow);
void           apiWindowDisableCursor(ApiWindow apiWindow);
Bool           apiWindowCursorIsCaptured(ApiWindow apiWindow);
Bool           apiWindowCursorIsHidden(ApiWindow apiWindow);
DVec2          apiWindowGetCursorPosition(ApiWindow apiWindow);
IVec2          apiWindowGetFramebufferSize(ApiWindow apiWindow);
IVec2          apiWindowGetWindowFrameSize(ApiWindow apiWindow);
Int            apiWindowGetFramebufferWidth(ApiWindow apiWindow);
Int            apiWindowGetFramebufferHeight(ApiWindow apiWindow);
Bool           apiWindowKeyIsPressed(ApiWindow apiWindow, Int key);
void           apiWindowSetExclusiveFullscreen(ApiWindow apiWindow);
void           apiWindowSetVisibility(ApiWindow apiWindow, Bool visible);
void           apiWindowResize(ApiWindow apiWindow, Int width, Int height);
void           whenWindowApiThrowsError(Int error, const Char* description);
void           apiSetWindowShouldClose(ApiWindow apiWindow, Bool shouldClose);
void           apiWindowSetCursorPosition(ApiWindow apiWindow, DVec2 position);
void           apiWindowSetStandardCursor(ApiWindow apiWindow, ApiCursorType cursor);
// Call before glfwTerminate(); cached standard cursors are invalid after GLFW shuts down.
void           apiReleaseStandardCursors();
ApiWindow      apiCreateWindow(IVec2 size, const Char* title, ApiWindow shared);
void           apiWindowSetWindowedMode(ApiWindow apiWindow, TiledRectangle monitorArea);
