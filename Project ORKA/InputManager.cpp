#include "InputManager.hpp"
#include "Window.hpp"


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
	if (action == GLFW_PRESS) {

		switch (keyID) {
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window.apiWindow, GLFW_TRUE);
			break;
		case GLFW_KEY_N: window.duplicateWindow = true;
			break;
		case GLFW_KEY_B: window.borderlessFullScreen = !window.borderlessFullScreen;
			break;
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case GLFW_KEY_J: window.renderer.adjustRenderVariables = !window.renderer.adjustRenderVariables;
			break;
		case GLFW_KEY_K: window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
			break;
		case GLFW_KEY_G: window.renderer.planetRenderSystem.chunkBorders = !window.renderer.planetRenderSystem.chunkBorders;
			break;
		case GLFW_KEY_T: {
			window.renderer.mutex.lock();
			window.renderer.shaderSystem.rebuild();
			window.renderer.mutex.unlock();
		}
			break;
		case GLFW_KEY_W: forward.isPressed = true;
			break;
		case GLFW_KEY_S: backward.isPressed = true;
			break;
		case GLFW_KEY_A: left.isPressed = true;
			break;
		case GLFW_KEY_D: right.isPressed = true;
			break;
		case GLFW_KEY_Q: down.isPressed = true;
			break;
		case GLFW_KEY_E: up.isPressed = true;
			break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (keyID) {
		case GLFW_KEY_W: forward.isPressed = false;
			break;
		case GLFW_KEY_S: backward.isPressed = false;
			break;
		case GLFW_KEY_A: left.isPressed = false;
			break;
		case GLFW_KEY_D: right.isPressed = false;
			break;
		case GLFW_KEY_Q: down.isPressed = false;
			break;
		case GLFW_KEY_E: up.isPressed = false;
			break;
		default:
			break;
		}
	}
}