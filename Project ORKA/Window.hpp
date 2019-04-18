#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "RenderingSystem.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "GLFW/glfw3.h"


//callback definitions
void whenWindowIsMoved(GLFWwindow* window, int xPos, int yPos);
void whenWindowIsResized(GLFWwindow* window, int width, int height);
void whenWindowChangedFocus(GLFWwindow* window, int focused);
void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos);
void whenMouseEnterWindow(GLFWwindow* window, int entered);
void whenMouseIsScrolling(GLFWwindow* window, double xoffset, double yoffset);
void whenMouseIsPressed(GLFWwindow* window, int button, int action, int mods);
void whenButtonIsPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

class Window {
public:
	unsigned int antiAliasing;
	bool insideWindow;
	bool capturingCursor;
	//settings
	std::string title;
	bool fullScreen;
	unsigned int width;
	unsigned int height;
	double ssCurPosX, ssCurPosY;
	double deltaX, deltaY;
	int winPosX, winPosY;

	GLFWwindow * glfwWindow;
	RenderingSystem * renderingSystem;
	InputHandler input;

	Window(RenderingSystem * renderingSystem) : renderingSystem(renderingSystem) {

		title = "Project ORKA"; //mountain strike
		fullScreen = false;
		capturingCursor = fullScreen;
		width = 1600;
		height = 900;
		antiAliasing = 4;
		//create the window
		makeTheWindow();
		printf("window was made\n");
		centerWindow();
	}
	void makeTheWindow() {
		setWindowHints();
		try {

			glfwWindow = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
			if (!glfwWindow) {
				throw std::exception("Failed to initialize Window!");
			}
		}
		catch (std::exception error) {
			std::cout << "Error: " << error.what() << std::endl;
			std::getchar();
			exit(EXIT_FAILURE);
		};
		
		glfwMakeContextCurrent(glfwWindow);

		debugPrint("Initializing GLEW...");
		try {
			if (glewInit() != GLEW_OK) {
				throw std::exception("Failed to initialize GLEW!");
			}
		}
		catch (std::exception error) {
			std::cout << "Error: " << error.what() << std::endl;
			std::getchar();
			exit(EXIT_FAILURE);
		};

		glfwSetWindowUserPointer(glfwWindow, this);

		glfwShowWindow(glfwWindow);

		if (fullScreen) {
			fullScreen = false;
			toggleFullscreen();
		}

		glfwSetCursorPos(glfwWindow,ssCurPosX, ssCurPosY);

		if (capturingCursor) {
			captureCursor();
		}
		else {
			uncaptureCursor();
		}



		glfwSetWindowPos(glfwWindow, winPosX, winPosY);
		setWindowCallbacks();
	}
	void destroyTheWindow() {
		glfwDestroyWindow(glfwWindow);
	}
	void reloadTheWindow() {
		destroyTheWindow();
		makeTheWindow();
	};
	void render() {
		glfwMakeContextCurrent(glfwWindow);

		renderingSystem->render(width, height);
		glfwSwapBuffers(glfwWindow);
	}
	void inputs() {
		glfwMakeContextCurrent(glfwWindow);

		input.forward.set(glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS);
		input.backward.set(glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS);
		input.right.set(glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS);
		input.left.set(glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS);
		input.down.set(glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS);
		input.up.set(glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS);
		input.escape.set(glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(glfwWindow));
		input.wireframe.set(glfwGetKey(glfwWindow, GLFW_KEY_F) == GLFW_PRESS);
	};
	void changeAntiAliasing(unsigned int antiAliasing) {
		this->antiAliasing = antiAliasing;
		reloadTheWindow();
	}
	void captureCursor() {
		glfwGetCursorPos(glfwWindow, &ssCurPosX, &ssCurPosY);
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		capturingCursor = true;
	}
	void uncaptureCursor() {
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(glfwWindow, ssCurPosX, ssCurPosY);
		deltaX = 0;
		deltaY = 0;
		capturingCursor = false;
	};
	void setWindowHints() {
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, antiAliasing);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	}
	void centerWindow() {
		winPosX = (glfwGetVideoMode(glfwGetPrimaryMonitor())->width - width) / 2;
		winPosY = (glfwGetVideoMode(glfwGetPrimaryMonitor())->height - height) / 2;
		glfwSetWindowPos(glfwWindow, winPosX, winPosY);
	}
	void toggleFullscreen() {
		fullScreen = !fullScreen;
		if (fullScreen) {
			glfwGetWindowPos(glfwWindow, &winPosX, &winPosY);
			glfwMaximizeWindow(glfwWindow);
			glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), winPosX, winPosY, width, height, GLFW_DONT_CARE);
		}
		else {
			glfwSetWindowMonitor(glfwWindow, nullptr, winPosX, winPosY, width, height, GLFW_DONT_CARE);
			glfwRestoreWindow(glfwWindow);
		}
	};
	void setWindowCallbacks() {
		glfwSetWindowPosCallback(glfwWindow, whenWindowIsMoved);
		glfwSetFramebufferSizeCallback(glfwWindow, whenWindowIsResized);
		glfwSetCursorPosCallback(glfwWindow, whenMouseIsMoving);
		glfwSetScrollCallback(glfwWindow, whenMouseIsScrolling);
		glfwSetKeyCallback(glfwWindow, whenButtonIsPressed);
		glfwSetMouseButtonCallback(glfwWindow, whenMouseIsPressed);
		glfwSetCursorEnterCallback(glfwWindow, whenMouseEnterWindow);
		glfwSetWindowFocusCallback(glfwWindow, whenWindowChangedFocus);
		glEnable(GL_DEBUG_OUTPUT);
		//glDebugMessageCallback(DebugOutputCallback, 0);
	}
	~Window() {
		destroyTheWindow();
	}
};

//callback definitions
void whenWindowIsMoved(GLFWwindow* window, int xPos, int yPos) {
	printf("window was moved\n");
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	parentWindowClass->winPosX = xPos;
	parentWindowClass->winPosY = yPos;
};
void whenWindowIsResized(GLFWwindow* window, int width, int height) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	parentWindowClass->width = width;
	parentWindowClass->height = height;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		parentWindowClass->centerWindow();
	}
	parentWindowClass->render();
}
void whenWindowChangedFocus(GLFWwindow* window, int focused) {

	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (focused) {
		if (parentWindowClass->capturingCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}
void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (parentWindowClass->insideWindow) {
		if (parentWindowClass->capturingCursor) {
			parentWindowClass->deltaX = xpos - parentWindowClass->ssCurPosX;
			parentWindowClass->deltaY = ypos - parentWindowClass->ssCurPosY;
			glfwSetCursorPos(window, parentWindowClass->ssCurPosX, parentWindowClass->ssCurPosY);
		}
		else {
			parentWindowClass->ssCurPosX = xpos;
			parentWindowClass->ssCurPosY = ypos;
		}
	}
	//get parent class
	//Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	//
	//if (!parentWindowClass->fullscreen) {
	//	static bool ignoreFirstTime = true;								//this code prevents weird camera rotation on windowed mode
	//	if (ignoreFirstTime) {
	//		ignoreFirstTime = false;
	//		glfwSetCursorPos(window, parentWindowClass->width / 2, parentWindowClass->height / 2);
	//		return;
	//	}
	//}
	//
	////get delta
	//float mouseX = (float)xpos - parentWindowClass->width / 2.0f;
	//float mouseY = (float)ypos - parentWindowClass->height / 2.0f;
	////apply delta to rotation
	//parentWindowClass->camera->rotateCamera(mouseX, mouseY);
	////recenter cursor
	//glfwSetCursorPos(window, parentWindowClass->width / 2, parentWindowClass->height / 2);
};
void whenMouseEnterWindow(GLFWwindow* window, int entered) {

	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (entered) {
		parentWindowClass->insideWindow = true;
	}
	else {
		parentWindowClass->insideWindow = false;
	}
};
void whenMouseIsScrolling(GLFWwindow* window, double xoffset, double yoffset) {

	//get parent class

	//Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	//
	//static int mvspeed = 1;
	//mvspeed += (int)yoffset;
	//parentWindowClass->camera->cameraMovementSpeed = pow(1.1f, mvspeed);
};
void whenMouseIsPressed(GLFWwindow* window, int button, int action, int mods) {
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			parentWindowClass->input.action.set(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		}
		else {
			parentWindowClass->captureCursor();
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass->capturingCursor) {
			parentWindowClass->uncaptureCursor();
		}
	}
};
void whenButtonIsPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		parentWindowClass->toggleFullscreen();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(parentWindowClass->glfwWindow, GLFW_TRUE);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(0);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(4);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(8);
	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(16);
	}
}
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	//if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
	//	printf("OpenGL Debug Output message : ");

	//	if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
	//	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
	//	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
	//	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
	//	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
	//	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

	//	if (type == GL_DEBUG_TYPE_ERROR_ARB)					printf("Type : ERROR; ");
	//	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
	//	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
	//	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
	//	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
	//	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

	//	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
	//	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
	//	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");
	//	printf("Message : %s\n", message);
	//	system("pause");
	//}
}
#endif // !WINDOW_HPP
