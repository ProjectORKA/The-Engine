
#include "Program.hpp"

void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos) {
	Window & parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (parentWindowClass.capturingCursor) {
		glfwGetCursorPos(parentWindowClass.glfwWindow, &parentWindowClass.deltaX, &parentWindowClass.deltaY);
		glfwSetCursorPos(parentWindowClass.glfwWindow, 0, 0);
		
		rotateCamera(parentWindowClass.camera, xpos, ypos);
	
	} else {
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
		parentWindowClass->renderer->gameServer->gameTime.paused = !parentWindowClass->renderer->gameServer->gameTime.paused;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		parentWindowClass->duplicateWindow = true;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		parentWindowClass->borderlessFullScreen = !parentWindowClass->borderlessFullScreen;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		parentWindowClass->renderer->wireframeMode = !parentWindowClass->renderer->wireframeMode;
	}
}

void whenMouseIsScrolling(GLFWwindow * window, double xoffset, double yoffset) {
	Window & parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));
	Camera & camera = parentWindowClass.camera;

	camera.speedMultiplier += (int)yoffset;
	camera.cameraSpeed = pow(1.1f, camera.speedMultiplier);
}

void whenMouseIsPressed(GLFWwindow * window, int button, int action, int mods) {
	Window & parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (parentWindowClass.capturingCursor) {
			
		}
		else {
			captureCursor(parentWindowClass);
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass.capturingCursor) {
			uncaptureCursor(parentWindowClass);
		}
	}
}

void whenWindowIsResized(GLFWwindow * window, int width, int height) {
	Window & parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));

	parentWindowClass.width = width;
	parentWindowClass.height = height;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		centerWindow(parentWindowClass);
	}
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

void processKeyboardInput(Window & window) {
	float delta = window.renderer->renderTime.getDelta();

	Camera & camera = window.camera;
	glm::vec3 totalAccelerationVector(0.0f);

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_E) == GLFW_PRESS) totalAccelerationVector += camera.upVector;
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) totalAccelerationVector -= camera.upVector;

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS) totalAccelerationVector += camera.forwardVector;
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS) totalAccelerationVector -= camera.forwardVector;

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS) totalAccelerationVector += camera.rightVector;
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS) totalAccelerationVector -= camera.rightVector;

	totalAccelerationVector *= camera.cameraSpeed * delta;

	camera.cameraLocation += totalAccelerationVector;
}