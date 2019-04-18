#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "RenderingSystem.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "GLFW/glfw3.h"
#include "Image.hpp"


//callback definitions
void whenWindowIsMoved(GLFWwindow* window, int xPos, int yPos);
void whenWindowIsResized(GLFWwindow* window, int width, int height);
void whenWindowChangedFocus(GLFWwindow* window, int focused);
void whenMouseIsMoving(GLFWwindow* window, double xpos, double ypos);
void whenMouseIsScrolling(GLFWwindow* window, double xoffset, double yoffset);
void whenMouseIsPressed(GLFWwindow* window, int button, int action, int mods);
void whenButtonIsPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

class Window {
public:
	unsigned int antiAliasing;
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
		fullScreen = true;
		capturingCursor = fullScreen;
		width = 1600;
		height = 900;
		antiAliasing = 4;

		createTheWindow();
		centerWindow();
	}
	void createTheWindow() {
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

		setIcon("icon.png");

		glfwShowWindow(glfwWindow);

		if (fullScreen) {
			fullScreen = false;
			toggleFullscreen();
		}

		glfwSetCursorPos(glfwWindow, ssCurPosX, ssCurPosY);

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		if (capturingCursor) {
			captureCursor();
		}
		else {
			uncaptureCursor();
		}

		glfwSetWindowPos(glfwWindow, winPosX, winPosY);

		setWindowCallbacks();
		debugPrint("Window was created!");
	}
	void destroyTheWindow() {
		glfwDestroyWindow(glfwWindow);
		debugPrint("Window was destroyed!");
	}
	void reloadTheWindow() {
		destroyTheWindow();
		createTheWindow();
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
		const GLFWvidmode * videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		glfwWindowHint(GLFW_SAMPLES, antiAliasing);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		//transparency
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
		//glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	}
	void centerWindow() {
		//figure out center of WORKABLE area
		int x, y, w, h;
		glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &w, &h);
		int centerOfWorkableAreaX = x + (w / 2);
		int centerofWorkableAreaY = y + (h / 2);
		//figure out top left cornder based on center
		winPosX = centerOfWorkableAreaX - (width / 2);
		winPosY = centerofWorkableAreaY - (height / 2);
		glfwSetWindowPos(glfwWindow, winPosX, winPosY);
	}
	void toggleFullscreen() {
		fullScreen = !fullScreen;
		if (fullScreen) {
			glfwGetWindowPos(glfwWindow, &winPosX, &winPosY);
			glfwMaximizeWindow(glfwWindow);
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, videoMode->width, videoMode->height, GLFW_DONT_CARE);
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
		glfwSetWindowFocusCallback(glfwWindow, whenWindowChangedFocus);
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(DebugOutputCallback, 0);
	}
	void setIcon(std::string path) {
		GLFWimage images[1];
		images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
		glfwSetWindowIcon(glfwWindow, 1, images);
		stbi_image_free(images[0].pixels);
	}
	~Window() {
		destroyTheWindow();
	}
};

//callback definitions
void whenWindowIsMoved(GLFWwindow* window, int xPos, int yPos) {
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
	if (glfwGetWindowAttrib(window, GLFW_HOVERED)) {
		if (parentWindowClass->capturingCursor) {
			parentWindowClass->deltaX = xpos - parentWindowClass->ssCurPosX;
			parentWindowClass->deltaY = ypos - parentWindowClass->ssCurPosY;
			glfwSetCursorPos(window, parentWindowClass->ssCurPosX, parentWindowClass->ssCurPosY);
			std::cout << "mouse ( " << parentWindowClass->deltaX << " / " << parentWindowClass->deltaY << " )" << std::endl;
		}
		else {
			parentWindowClass->ssCurPosX = xpos;
			parentWindowClass->ssCurPosY = ypos;
		}
	}
	//parentWindowClass->camera->rotateCamera(mouseX, mouseY);
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

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
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
	if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
	
	}
}
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		printf("OpenGL Debug Output message : ");

		if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

		if (type == GL_DEBUG_TYPE_ERROR_ARB)					printf("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");
		printf("Message : %s\n", message);
		system("pause");
	}
}
#endif // !WINDOW_HPP
