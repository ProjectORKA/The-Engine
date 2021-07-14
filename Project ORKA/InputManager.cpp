#include "InputManager.hpp"
#include "Window.hpp"


InputManager inputManager;

Bool InputManager::isCapturing(Window& window)
{
	return apiWindowIsCapturing(window.apiWindow);
}
void InputManager::captureCursor(Window& window)
{
	logDebug("capturing!");
	if (!capturing) {
		captureReleasePosition = cursorPosition;
		apiWindowDisableCursor(window.apiWindow);
		apiWindowSetCursorPosition(window.apiWindow, Vec2(0, 0));
		cursorPosition = Vec2(0, 0);
		capturing = true;
	}
}
void InputManager::uncaptureCursor(Window& window)
{
	logDebug("release!");
	if (capturing) {
		//[TODO]
		apiWindowEnableCursor(window.apiWindow);
		glfwSetCursorPos(window.apiWindow, captureReleasePosition.x, captureReleasePosition.y);
		cursorPosition = captureReleasePosition;
		cursorDelta = IVec2(0);
		capturing = false;
	}
}

void InputManager::windowInFocus(Window& window)
{
	uncaptureCursor(window);
}
void InputManager::windowOutOfFocus(Window& window)
{
	//captureCursor(window);
}

//callbacks
void InputManager::mouseIsMoving(Window& window, IVec2 position)
{
	cursorPosition = position;
}
void InputManager::windowChangedFocus(Window& window, Bool isInFocus)
{
	if (capturing)uncaptureCursor(window);
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
		case GLFW_KEY_K: window.renderer.planetRenderSystem.quadtreeRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.quadtreeRenderSystem.worldDistortion;
			break;
		case GLFW_KEY_G: window.renderer.planetRenderSystem.quadtreeRenderSystem.chunkBorders = !window.renderer.planetRenderSystem.quadtreeRenderSystem.chunkBorders;
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