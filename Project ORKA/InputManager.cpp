#include "InputManager.hpp"
#include "WindowSystem.hpp"

InputManager inputManager;

Bool InputManager::isCapturing(Window& window)
{
	return apiWindowIsCapturing(window.apiWindow);
}
void InputManager::captureCursor(Window& window)
{
	if (!capturing) {
		capturePosition = cursorPosition;
		apiWindowDisableCursor(window.apiWindow);
		capturing = true;
	}
}
void InputManager::windowInFocus(Window& window)
{
	captureCursor(window);
}
void InputManager::uncaptureCursor(Window& window)
{
	if (capturing) {
		//[TODO]
		apiWindowEnableCursor(window.apiWindow);
		glfwSetCursorPos(window.apiWindow, cursorPosition.x, Int(window.getWindowContentSize().y) - cursorPosition.y);
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
	IVec2 normalizedPosition;
	normalizedPosition.x = position.x;
	normalizedPosition.y = window.getWindowContentSize().y - position.y;

	//cursorDelta = cursorPosition - normalizedPosition;
	cursorPosition = normalizedPosition;
}
void InputManager::windowChangedFocus(Window& window, Bool isInFocus)
{
	if (isInFocus)windowInFocus(window); else windowOutOfFocus(window);
}
void InputManager::mouseWheelIsScrolled(Window& window, Double xAxis, Double yAxis)
{
	scrollAxisYTotal += yAxis;
}
void InputManager::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers)
{
	switch (keyID) {
	case GLFW_KEY_ENTER:
		if (action == GLFW_PRESS && modifiers == GLFW_MOD_ALT) {
			if (window.isFullScreen()) window.setWindowed();
			else window.setExclusiveFullscreen();
		}
		break;
	case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window.apiWindow, GLFW_TRUE);
		break;
	case GLFW_KEY_N: window.duplicateWindow = true;
		break;
	case GLFW_KEY_B: window.borderlessFullScreen = !window.borderlessFullScreen;
		break;
	default:
		break;
	}

	window.userInterface.buttonIsPressed(window, keyID, action, modifiers);
}

