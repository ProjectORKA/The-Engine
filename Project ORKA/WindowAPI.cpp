
#include "WindowAPI.hpp"
#include "Util.hpp"

APIWindow apiCreateWindow(Area size, const Char* title) {
	return glfwCreateWindow(size.x, size.y, title, NULL, NULL);
}

void apiWindowRestore(APIWindow apiWindow)
{
	glfwRestoreWindow(apiWindow);
}
void apiMaximizeWindow(APIWindow apiWindow) {
	glfwMaximizeWindow(apiWindow);
}
void apiWindowDecorate(APIWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, true);
}
void apiMinimizeWindow(APIWindow apiWindow) {
	glfwIconifyWindow(apiWindow);
}
void apiWindowUndecorate(APIWindow apiWindow)
{
	glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, false);
}
void apiWindowSwapBuffers(APIWindow apiWindow)
{
	glfwSwapBuffers(apiWindow);
}
void apiWindowEnableCursor(APIWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void apiWindowDisableCursor(APIWindow apiWindow)
{
	glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void apiWindowSetExclusiveFullscreen(APIWindow apiWindow)
{
	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
}
void apiWindowSetVisibility(APIWindow apiWindow, Bool visible) {
	if (visible)glfwShowWindow(apiWindow);
	else glfwHideWindow(apiWindow);
}
void apiWindowResize(APIWindow apiWindow, Int width, Int height) {
	glfwSetWindowSize(apiWindow, width, height);
}
void whenWindowAPIThrowsError(Int error, const Char* description)
{
	logError(description);
}
void apiWindowSetCursorPosition(APIWindow apiWindow, Vec2 position)
{
	glfwSetCursorPos(apiWindow, position.x, position.y);
}
void apiWindowSetWindowedMode(APIWindow apiWindow, TiledRectangle monitorArea)
{
	glfwSetWindowMonitor(apiWindow, nullptr, monitorArea.position.x, monitorArea.position.y, monitorArea.size.x, monitorArea.size.y, GLFW_DONT_CARE);
}

Bool apiWindowShouldClose(APIWindow apiWindow)
{
	return glfwWindowShouldClose(apiWindow);
}
Bool apiWindowIsCapturing(APIWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
Bool apiWindowIsDecorated(APIWindow apiWindow)
{
	return glfwGetWindowAttrib(apiWindow, GLFW_DECORATED);
}
Bool apiWindowIsFullscreen(APIWindow apiWindow)
{
	return glfwGetWindowMonitor(apiWindow) != NULL;
}
Bool apiWindowCursorIsCaptured(APIWindow apiWindow)
{
	return glfwGetInputMode(apiWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
Bool apiWindowKeyIsPressed(APIWindow apiWindow, Key key)
{
	return glfwGetKey(apiWindow, enumClassAsInt(key)) == GLFW_PRESS;
}

Int apiWindowGetFramebufferWidth(APIWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return width;
}
Int apiWindowGetFramebufferHeight(APIWindow apiWindow)
{
	Int width, height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	return height;
}

DVec2 apiWindowGetCursorPosition(APIWindow apiWindow)
{
	DVec2 cursorPosition;
	glfwGetCursorPos(apiWindow, &cursorPosition.x, &cursorPosition.y);
	return cursorPosition;
}
Area apiWindowGetFramebufferSize(APIWindow apiWindow)
{
	Area size;
	glfwGetFramebufferSize(apiWindow, &size.x, &size.y);
	return size;
}
Area apiWindowGetWindowFrameSize(APIWindow apiWindow)
{
	Area area;
	glfwGetWindowSize(apiWindow, &area.x, &area.y);
	return area;
}
TiledRectangle apiWindowGetWorkableArea(APIWindow apiWindow)
{
	TiledRectangle rect;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &rect.position.x, &rect.position.y, &rect.size.x, &rect.size.y);
	return rect;
}

