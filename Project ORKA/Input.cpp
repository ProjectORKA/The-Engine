
#include "Program.hpp"

void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos) {
	Window & parentWindowClass = *static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (parentWindowClass.capturingCursor) {
		glfwGetCursorPos(parentWindowClass.glfwWindow, &parentWindowClass.deltaX, &parentWindowClass.deltaY);
		glfwSetCursorPos(parentWindowClass.glfwWindow, 0, 0);
		setAxis(parentWindowClass.input.yaw, parentWindowClass.deltaX);
		setAxis(parentWindowClass.input.pitch, parentWindowClass.deltaY);
	}
	else {
		glfwGetCursorPos(parentWindowClass.glfwWindow, &parentWindowClass.curPosX, &parentWindowClass.curPosY);
	}
}

void whenButtonIsPressed(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		toggleFullscreen(*parentWindowClass);
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(parentWindowClass->glfwWindow, GLFW_TRUE);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		changeAntiAliasing(*parentWindowClass, 0);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		changeAntiAliasing(*parentWindowClass, 8);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		parentWindowClass->renderingSystem->gameServer->time.paused = !parentWindowClass->renderingSystem->gameServer->time.paused;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		parentWindowClass->duplicateWindow = true;
	}
}

void whenMouseIsScrolling(GLFWwindow * window, double xoffset, double yoffset) {

	//get parent class

	//Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	//
	//static int mvspeed = 1;
	//mvspeed += (int)yoffset;
	//parentWindowClass->camera->cameraMovementSpeed = pow(1.1f, mvspeed);
}

void whenMouseIsPressed(GLFWwindow * window, int button, int action, int mods) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			//setKey(parentWindowClass->input.action, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		}
		else {
			captureCursor(*parentWindowClass);
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			uncaptureCursor(*parentWindowClass);
		}
	}
}

void whenWindowIsResized(GLFWwindow * window, int width, int height) {
	Window & parentWindowClass = *(static_cast<Window *>(glfwGetWindowUserPointer(window)));
	parentWindowClass.width = width;
	parentWindowClass.height = height;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		centerWindow(parentWindowClass);
	}
}


//void setKey(Key & key, bool pressed) {
//	if (key.holding != pressed) {
//		key.toggled = true;
//	}
//
//	if (key.toggled && pressed) {
//		key.activated = true;
//	}
//	else {
//		key.activated = false;
//	}
//
//	if (key.activated) {
//		key.toggleStatus = !key.toggleStatus;
//	}
//
//	key.holding = pressed;
//}
//
//void resetKey(Key & key) {
//	key.holding = false;
//	key.activated = false;
//	key.toggled = false;
//	key.toggleStatus = false;
//}


void setAxis(Axis & axis, double value) {
	axis.previous = axis.current;
	axis.current = value;
	axis.delta = axis.current - axis.previous;
}


void captureCursor(Window & window) {
	if (glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		glfwGetCursorPos(window.glfwWindow, &window.curPosX, &window.curPosY);
		glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window.glfwWindow, 0, 0);
		window.capturingCursor = true;
	}
}

void uncaptureCursor(Window & window) {
	if (glfwGetInputMode(window.glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		glfwSetInputMode(window.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window.glfwWindow, window.curPosX, window.curPosY);
		window.capturingCursor = false;
	}
}