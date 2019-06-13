
#include "Program.hpp"

void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos) {
	Window & parentWindowClass = *static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (parentWindowClass.capturingCursor) {
		glfwGetCursorPos(parentWindowClass.glfwWindow, &parentWindowClass.deltaX, &parentWindowClass.deltaY);
		glfwSetCursorPos(parentWindowClass.glfwWindow, 0, 0);
		
		rotateCamera(*parentWindowClass.cameraSystem, parentWindowClass.renderingSystem->cameraID, xpos, ypos);
	
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
		parentWindowClass->renderingSystem->gameServer->gameTime.paused = !parentWindowClass->renderingSystem->gameServer->gameTime.paused;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		parentWindowClass->duplicateWindow = true;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		parentWindowClass->borderlessFullScreen = !parentWindowClass->borderlessFullScreen;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		parentWindowClass->renderingSystem->wireframeMode = !parentWindowClass->renderingSystem->wireframeMode;
	}
}

void whenMouseIsScrolling(GLFWwindow * window, double xoffset, double yoffset) {
	Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	
	static int mvspeed = 24; //1.1 pow 24 is closest to 10.0
	mvspeed += (int)yoffset;
	parentWindowClass->cameraSystem->cameraSpeed[parentWindowClass->renderingSystem->cameraID] = pow(1.1f, mvspeed);
}

void whenMouseIsPressed(GLFWwindow * window, int button, int action, int mods) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			
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
	float delta = window.renderingSystem->gameServer->serverTime.getDelta();

	unsigned int cameraID = window.renderingSystem->cameraID;
	CameraSystem & cameraSystem = *window.cameraSystem;
	glm::vec3 totalAccelerationVector(0.0f);

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_E) == GLFW_PRESS) totalAccelerationVector += cameraSystem.upVector[cameraID];
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) totalAccelerationVector -= cameraSystem.upVector[cameraID];

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_W) == GLFW_PRESS) totalAccelerationVector += cameraSystem.forwardVector[cameraID];
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_S) == GLFW_PRESS) totalAccelerationVector -= cameraSystem.forwardVector[cameraID];

	if (glfwGetKey(window.glfwWindow, GLFW_KEY_D) == GLFW_PRESS) totalAccelerationVector += cameraSystem.rightVector[cameraID];
	if (glfwGetKey(window.glfwWindow, GLFW_KEY_A) == GLFW_PRESS) totalAccelerationVector -= cameraSystem.rightVector[cameraID];

	totalAccelerationVector *= cameraSystem.cameraSpeed[cameraID] * delta;

	cameraSystem.cameraLocation[cameraID] += totalAccelerationVector;
}