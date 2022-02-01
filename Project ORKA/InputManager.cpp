#include "InputManager.hpp"
#include "Window.hpp"
#include "UserInterface.hpp"
#include "FileSystem.hpp"

InputManager inputManager;

Bool InputManager::isCapturing(Window& window)
{
	return apiWindowIsCapturing(window.apiWindow);
}
void InputManager::captureCursor(Window& window)
{
	if (!capturing) {
		capturePosition = apiWindowGetCursorPosition(window.apiWindow);
		apiWindowSetCursorPosition(window.apiWindow, Vec2(0));
		cursorPosition = Vec2(0);
		apiWindowDisableCursor(window.apiWindow);
		capturing = true;
	}
}
void InputManager::windowInFocus(Window& window)
{
	//captureCursor(window);
}
void InputManager::uncaptureCursor(Window& window)
{
	if (capturing) {
		//[TODO]
		apiWindowEnableCursor(window.apiWindow);
		glfwSetCursorPos(window.apiWindow, capturePosition.x, capturePosition.y);
		//cursorPosition = capturePosition;
		capturing = false;
	}
}
void InputManager::windowOutOfFocus(Window& window)
{
	//uncaptureCursor(window);
}
void InputManager::mouseIsMoving(Window& window, IVec2 position)
{
	if (apiWindowCursorIsCaptured(window.apiWindow)) {
		cursorPosition = IVec2(position.x, -position.y);
		apiWindowSetCursorPosition(window.apiWindow,Vec2(0));
	}
	else {
		cursorPosition = IVec2(position.x, window.getWindowContentSize().y - position.y);
	}

	if(ui.currentlyActive) ui.currentlyActive->mouseIsMoving(window, position);
}
void InputManager::filesDropped(Window& window, Vector<Path> paths) {
	if(window.content)window.content->filesDropped(window, paths);
}
void InputManager::windowChangedFocus(Window& window, Bool isInFocus)
{
	if (isInFocus)windowInFocus(window); else windowOutOfFocus(window);
}
void InputManager::mouseWheelIsScrolled(Window& window, Double xAxis, Double yAxis)
{
	window.content->mouseIsScrolled(window, xAxis, yAxis);
}
void InputManager::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers)
{
	if (action == GLFW_PRESS) {
		switch (keyID) {
		case GLFW_KEY_ENTER:
			if (modifiers == GLFW_MOD_ALT) {
				if (window.windowState == Window::windowed) window.windowState = Window::fullscreen;
				else window.windowState = Window::windowed;
				window.updateWindowState();
			}
			break;
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window.apiWindow, GLFW_TRUE);
			break;
		case GLFW_KEY_B:
			if (modifiers == GLFW_MOD_ALT) {
				if (window.decorated) window.undecorateWindow();
				else window.decorateWindow();
			}
			break;
		default:
			break;
		}
	}
	
	if(window.content)window.content->buttonIsPressed(window, keyID, action, modifiers);
}
void InputManager::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if(window.content)window.content->mouseIsPressed(window, button, action, modifiers);
}