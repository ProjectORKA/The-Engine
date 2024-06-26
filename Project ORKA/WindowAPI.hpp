#pragma once

#include "TiledMath.hpp"
#include "KeyMap.hpp"

using APIWindow = GLFWwindow*;
using APIMonitor = GLFWmonitor*;

void           apiWindowRestore(APIWindow apiWindow);
void           apiMaximizeWindow(APIWindow apiWindow);
void           apiWindowDecorate(APIWindow apiWindow);
void           apiMinimizeWindow(APIWindow apiWindow);
void           apiWindowUndecorate(APIWindow apiWindow);
Bool           apiWindowIsCapturing(APIWindow apiWindow);
Bool           apiWindowIsDecorated(APIWindow apiWindow);
void           apiWindowSwapBuffers(APIWindow apiWindow);
void           apiWindowEnableCursor(APIWindow apiWindow);
Bool           apiWindowIsFullscreen(APIWindow apiWindow);
void           apiWindowDisableCursor(APIWindow apiWindow);
TiledRectangle apiWindowGetWorkableArea(APIWindow apiWindow);
Bool           apiWindowCursorIsCaptured(APIWindow apiWindow);
DVec2          apiWindowGetCursorPosition(APIWindow apiWindow);
Area           apiWindowGetFramebufferSize(APIWindow apiWindow);
Area           apiWindowGetWindowFrameSize(APIWindow apiWindow);
Int            apiWindowGetFramebufferWidth(APIWindow apiWindow);
Int            apiWindowGetFramebufferHeight(APIWindow apiWindow);
Bool           apiWindowKeyIsPressed(APIWindow apiWindow, Int key);
void           apiWindowSetExclusiveFullscreen(APIWindow apiWindow);
void           apiWindowSetVisibility(APIWindow apiWindow, Bool visible);
void           apiWindowResize(APIWindow apiWindow, Int width, Int height);
void           whenWindowAPIThrowsError(Int error, const Char* description);
void           apiSetWindowShouldClose(APIWindow apiWindow, Bool shouldClose);
void           apiWindowSetCursorPosition(APIWindow apiWindow, DVec2 position);
APIWindow      apiCreateWindow(Area size, const Char* title, APIWindow shared);
void           apiWindowSetWindowedMode(APIWindow apiWindow, TiledRectangle monitorArea);
