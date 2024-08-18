#include "WindowAPI.hpp"
#include "Debug.hpp"

void apiWindowRestore(const ApiWindow apiWindow)
{
	glfwRestoreWindow(apiWindow);
}

void apiMaximizeWindow(const ApiWindow apiWindow)
{
	glfwMaximizeWindow(apiWindow);
}

void apiWindowDecorate(const ApiWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, true);
}

void apiMinimizeWindow(const ApiWindow apiWindow)
{
	glfwIconifyWindow(apiWindow);
}

void apiWindowUndecorate(const ApiWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, false);
}

void apiWindowSwapBuffers(const ApiWindow apiWindow)
{
	glfwSwapBuffers(apiWindow);
}

Bool apiWindowIsCapturing(const ApiWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Bool apiWindowIsDecorated(const ApiWindow apiWindow)
{
	return glfwGetWindowAttrib(apiWindow, GLFW_DECORATED);
}

Bool apiWindowIsFullscreen(const ApiWindow apiWindow)
{
	return glfwGetWindowMonitor(apiWindow) != nullptr;
}

void apiWindowEnableCursor(const ApiWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void apiWindowDisableCursor(const ApiWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Bool apiGetWindowShouldClose(const ApiWindow apiWindow)
{
	return glfwWindowShouldClose(apiWindow);
}

Bool apiWindowCursorIsCaptured(const ApiWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

Int apiWindowGetFramebufferWidth(const ApiWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return width;
}

DVec2 apiWindowGetCursorPosition(const ApiWindow apiWindow)
{
	DVec2 cursorPosition;
	glfwGetCursorPos(apiWindow, &cursorPosition.x, &cursorPosition.y);
	return cursorPosition;
}

Area apiWindowGetFramebufferSize(const ApiWindow apiWindow)
{
	Area size;
	glfwGetFramebufferSize(apiWindow, &size.x, &size.y);
	return size;
}

Area apiWindowGetWindowFrameSize(const ApiWindow apiWindow)
{
	Area area;
	glfwGetWindowSize(apiWindow, &area.x, &area.y);
	return area;
}

Int apiWindowGetFramebufferHeight(const ApiWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return height;
}

TiledRectangle apiWindowGetWorkableArea(ApiWindow apiWindow)
{
	TiledRectangle rect;
	External::glfwGetMonitorWorkarea(External::glfwGetPrimaryMonitor(), &rect.position.x, &rect.position.y, &rect.size.x, &rect.size.y);
	return rect;
}

void apiWindowSetExclusiveFullscreen(const ApiWindow apiWindow)
{
	glfwSetWindowMonitor(apiWindow, External::glfwGetPrimaryMonitor(), 0, 0, External::glfwGetVideoMode(External::glfwGetPrimaryMonitor())->width, External::glfwGetVideoMode(External::glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
}

void whenWindowApiThrowsError(Int error, const Char* description)
{
	logError(description);
}

Bool apiWindowKeyIsPressed(const ApiWindow apiWindow, const Int key)
{
	return glfwGetKey(apiWindow, key) > 0;
}

void apiWindowSetVisibility(const ApiWindow apiWindow, const Bool visible)
{
	if(visible)
	{
		glfwShowWindow(apiWindow);
	}
	else
	{
		glfwHideWindow(apiWindow);
	}
}

void apiSetWindowShouldClose(const ApiWindow apiWindow, const Bool shouldClose)
{
	glfwSetWindowShouldClose(apiWindow, shouldClose);
}

void apiWindowResize(const ApiWindow apiWindow, const Int width, const Int height)
{
	glfwSetWindowSize(apiWindow, width, height);
}

ApiWindow apiCreateWindow(const Area size, const Char* title, const ApiWindow shared)
{
	return glfwCreateWindow(size.x, size.y, title, nullptr, shared);
}

void apiWindowSetCursorPosition(const ApiWindow apiWindow, const DVec2 position)
{
	glfwSetCursorPos(apiWindow, position.x, position.y);
}

void apiWindowSetWindowedMode(const ApiWindow apiWindow, const TiledRectangle monitorArea)
{
	glfwSetWindowMonitor(apiWindow, nullptr, monitorArea.position.x, monitorArea.position.y, monitorArea.size.x, monitorArea.size.y, GLFW_DONT_CARE);
}
