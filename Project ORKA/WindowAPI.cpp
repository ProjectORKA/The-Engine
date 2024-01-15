#include "WindowAPI.hpp"
#include "Debug.hpp"
#include "Profiler.hpp"

void apiWindowRestore(const APIWindow apiWindow)
{
	glfwRestoreWindow(apiWindow);
}

void apiMaximizeWindow(const APIWindow apiWindow)
{
	glfwMaximizeWindow(apiWindow);
}

void apiWindowDecorate(const APIWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, true);
}

void apiMinimizeWindow(const APIWindow apiWindow)
{
	glfwIconifyWindow(apiWindow);
}

void apiWindowUndecorate(const APIWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, false);
}

void apiWindowSwapBuffers(const APIWindow apiWindow)
{
	glfwSwapBuffers(apiWindow);
}

Bool apiWindowIsCapturing(const APIWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Bool apiWindowIsDecorated(const APIWindow apiWindow)
{
	return glfwGetWindowAttrib(apiWindow, GLFW_DECORATED);
}

Bool apiWindowIsFullscreen(const APIWindow apiWindow)
{
	return glfwGetWindowMonitor(apiWindow) != nullptr;
}

void apiWindowEnableCursor(const APIWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void apiWindowDisableCursor(const APIWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Bool apiGetWindowShouldClose(const APIWindow apiWindow)
{
	return glfwWindowShouldClose(apiWindow);
}

Bool apiWindowCursorIsCaptured(const APIWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Int apiWindowGetFramebufferWidth(const APIWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return width;
}

DVec2 apiWindowGetCursorPosition(const APIWindow apiWindow)
{
	DVec2 cursorPosition;
	glfwGetCursorPos(apiWindow, &cursorPosition.x, &cursorPosition.y);
	return cursorPosition;
}

Area apiWindowGetFramebufferSize(const APIWindow apiWindow)
{
	Area size;
	glfwGetFramebufferSize(apiWindow, &size.x, &size.y);
	return size;
}

Area apiWindowGetWindowFrameSize(const APIWindow apiWindow)
{
	Area area;
	glfwGetWindowSize(apiWindow, &area.x, &area.y);
	return area;
}

Int apiWindowGetFramebufferHeight(const APIWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return height;
}

TiledRectangle apiWindowGetWorkableArea(APIWindow apiWindow)
{
	TiledRectangle rect;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &rect.position.x, &rect.position.y, &rect.size.x, &rect.size.y);
	return rect;
}

void apiWindowSetExclusiveFullscreen(const APIWindow apiWindow)
{
	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
}

void whenWindowAPIThrowsError(Int error, const Char* description)
{
	logError(description);
}

Bool apiWindowKeyIsPressed(const APIWindow apiWindow, const Int key)
{
	return glfwGetKey(apiWindow, key) > 0;
}

void apiWindowSetVisibility(const APIWindow apiWindow, const Bool visible)
{
	if(visible) glfwShowWindow(apiWindow);
	else glfwHideWindow(apiWindow);
}

void apiSetWindowShouldClose(const APIWindow apiWindow, const Bool shouldClose)
{
	glfwSetWindowShouldClose(apiWindow, shouldClose);
}

void apiWindowResize(const APIWindow apiWindow, const Int width, const Int height)
{
	glfwSetWindowSize(apiWindow, width, height);
}

APIWindow apiCreateWindow(const Area size, const Char* title, const APIWindow shared)
{
	return glfwCreateWindow(size.x, size.y, title, nullptr, shared);
}

void apiWindowSetCursorPosition(const APIWindow apiWindow, const MouseMovement position)
{
	glfwSetCursorPos(apiWindow, position.x, position.y);
}

void apiWindowSetWindowedMode(const APIWindow apiWindow, const TiledRectangle monitorArea)
{
	glfwSetWindowMonitor(apiWindow, nullptr, monitorArea.position.x, monitorArea.position.y, monitorArea.size.x, monitorArea.size.y, GLFW_DONT_CARE);
}
