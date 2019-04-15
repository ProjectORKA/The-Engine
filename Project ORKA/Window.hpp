#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "RenderingSystem.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "GLFW/glfw3.h"

class WindowSettings {
public:
	std::string title;
	bool fullScreen;
	unsigned int width;
	unsigned int height;
	unsigned int antiAliasing;
	WindowSettings() {
		title = "Project ORKA"; //mountain strike
		fullScreen = false;
		width = 1600;
		height = 900;
		antiAliasing = 4;
	}
};

//callback definitions
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
	GLFWwindow * glfwWindow;
	WindowSettings settings;
	RenderingSystem & renderingSystem;
	InputHandler input;
	bool capturingCursor;
	bool insideWindow;
	double ssCurPosX, ssCurPosY;
	int winPosX, winPosY;

	Window(RenderingSystem & renderingSystem) : renderingSystem(renderingSystem)  {
		capturingCursor = settings.fullScreen;
		setWindowHints();
		try {

			glfwWindow = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), NULL, NULL);
			if (!glfwWindow) {
				throw std::exception("Failed to initialize Window!");
			}
		}
		catch (std::exception error) {
			std::cout << "Error: " << error.what() << std::endl;
			std::getchar();
			exit(EXIT_FAILURE);
		};
		
		glfwSetWindowUserPointer(glfwWindow, this);
		
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

		centerWindow();
		//glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);				   [TODO] check for bugs related to this
		//glfwSetInputMode(glfwWindow, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);		   [TODO] check for bugs related to this
		setWindowCallbacks();
		glfwShowWindow(glfwWindow);

		glfwMaximizeWindow(glfwWindow);
		if (settings.fullScreen) {
			toggleFullscreen();
		}
	}
	void render() {
		glfwMakeContextCurrent(glfwWindow);
		
		renderingSystem.render(settings.width, settings.height);
		glfwSwapBuffers(glfwWindow);
	}
	void setWindowHints() {
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, settings.antiAliasing);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	}
	void centerWindow() {
		glfwSetWindowPos(glfwWindow, (glfwGetVideoMode(glfwGetPrimaryMonitor())->width - settings.width) / 2, (glfwGetVideoMode(glfwGetPrimaryMonitor())->height - settings.height) / 2);
	}
	void toggleFullscreen() {
		settings.fullScreen = !settings.fullScreen;
		if (settings.fullScreen) {
			glfwGetWindowPos(glfwWindow, &winPosX, &winPosY);
			glfwMaximizeWindow(glfwWindow);
			glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), winPosX, winPosY, settings.width, settings.height, GLFW_DONT_CARE);
		}
		else {
			glfwSetWindowMonitor(glfwWindow, nullptr, winPosX, winPosY, settings.width, settings.height, GLFW_DONT_CARE);
			glfwRestoreWindow(glfwWindow);
		}
	};
	void setWindowCallbacks() {
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
		glfwDestroyWindow(glfwWindow);
	}
};

//callback definitions
void whenWindowIsResized(GLFWwindow* window, int width, int height) {
	printf("window resized\n");
	//get parent class
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));

	parentWindowClass->settings.width = width;
	parentWindowClass->settings.height = height;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		parentWindowClass->centerWindow();
	}
	parentWindowClass->render();
}
void whenWindowChangedFocus(GLFWwindow* window, int focused) {
	printf("window changed focus\n");
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
			double deltaX = xpos - parentWindowClass->ssCurPosX;
			double deltaY = ypos - parentWindowClass->ssCurPosY;
			std::cout << "mm: (" << deltaX << "/" << deltaY << ")" << std::endl;
			glfwSetCursorPos(window, parentWindowClass->ssCurPosX, parentWindowClass->ssCurPosY);
		}
		else {
			std::cout << "mm: (" << xpos << "/" << ypos << ")" << std::endl;
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
	printf("mouse entered/left window\n");
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (entered) {
		parentWindowClass->insideWindow = true;
	}
	else {
		parentWindowClass->insideWindow = false;
	}
};
void whenMouseIsScrolling(GLFWwindow* window, double xoffset, double yoffset) {
	printf("mouse scrolled\n");
	//get parent class

	//Window *parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	//
	//static int mvspeed = 1;
	//mvspeed += (int)yoffset;
	//parentWindowClass->camera->cameraMovementSpeed = pow(1.1f, mvspeed);
};
void whenMouseIsPressed(GLFWwindow* window, int button, int action, int mods) {
	printf("mouse pressed\n");
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	//parentWindowClass.
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			parentWindowClass->input.action.set(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		}
		else {
			glfwGetCursorPos(window, &(parentWindowClass->ssCurPosX), &(parentWindowClass->ssCurPosY));
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			parentWindowClass->capturingCursor = true;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			parentWindowClass->capturingCursor = false;
			//glfwSetCursorPos(window, parentWindowClass->ssCurPosX, parentWindowClass->ssCurPosY);
		}
	}
};
void whenButtonIsPressed(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window * parentWindowClass = static_cast<Window *>(glfwGetWindowUserPointer(window));
	printf("button pressed\n");
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {// && mods == GLFW_MOD_ALT) {
		parentWindowClass->toggleFullscreen();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {// && mods == GLFW_MOD_ALT) {
		glfwSetWindowShouldClose(parentWindowClass->glfwWindow, GLFW_TRUE);
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
