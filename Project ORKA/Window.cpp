
#include "Window.hpp"
#include "Image.hpp"

Window::Window(RenderingSystem * renderingSystem) : renderingSystem(renderingSystem) {
	title = "Project ORKA"; //mountain strike
	fullScreen = false;
	borderlessFullScreen = false;
	capturingCursor = false;
	width = 1600;
	height = 900;
	antiAliasing = 4;
	createTheWindow();
	centerWindow();
}
Window::~Window() {
	destroyTheWindow();
}
void Window::createTheWindow() {
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

	renderingSystem->setDebugMessageCallback();

	glfwSetWindowUserPointer(glfwWindow, this);

	setIcon("icon.png");

	glfwShowWindow(glfwWindow);

	if (fullScreen) {
		fullScreen = false;
		toggleFullscreen();
	}

	if (glfwRawMouseMotionSupported()) glfwSetInputMode(glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	capturingCursor ? captureCursor() : uncaptureCursor();

	//if (capturingCursor) {
	//	captureCursor();
	//}
	//else {
	//	uncaptureCursor();
	//}

	glfwSetWindowPos(glfwWindow, winPosX, winPosY);

	setWindowCallbacks();
	debugPrint("Window was created!");
}
void Window::destroyTheWindow() {
	glfwDestroyWindow(glfwWindow);
	debugPrint("Window was destroyed!");
}
void Window::reloadTheWindow() {
	destroyTheWindow();
	createTheWindow();
};
void Window::render() {
	glfwMakeContextCurrent(glfwWindow);
	renderingSystem->render();
	glfwSwapBuffers(glfwWindow);
}
void Window::inputs() {
	glfwMakeContextCurrent(glfwWindow);

	input.forward.set(glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS);
	input.backward.set(glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS);
	input.right.set(glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS);
	input.left.set(glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS);
	input.down.set(glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS);
	input.up.set(glfwGetKey(glfwWindow, GLFW_KEY_E) == GLFW_PRESS);
	input.escape.set(glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(glfwWindow));
	input.wireframe.set(glfwGetKey(glfwWindow, GLFW_KEY_F) == GLFW_PRESS);


	if (capturingCursor) {
		glfwGetCursorPos(glfwWindow, &deltaX, &deltaY);
		glfwSetCursorPos(glfwWindow, 0, 0);
		input.yaw.set(deltaX);
		input.pitch.set(deltaY);
	}
	else {
		glfwGetCursorPos(glfwWindow, &curPosX, &curPosY);
	}
};
void Window::changeAntiAliasing(unsigned int antiAliasing) {
	this->antiAliasing = antiAliasing;
	reloadTheWindow();
}
void Window::captureCursor() {
	if (glfwGetInputMode(glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		glfwGetCursorPos(glfwWindow, &curPosX, &curPosY);
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(glfwWindow, 0, 0);
		capturingCursor = true;
	}
}
void Window::uncaptureCursor() {
	if (glfwGetInputMode(glfwWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(glfwWindow, curPosX, curPosY);
		capturingCursor = false;
	}
};
void Window::setWindowHints() {
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
void Window::centerWindow() {
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
void Window::toggleFullscreen() {
	fullScreen = !fullScreen;
	if (fullScreen) {
		glfwGetWindowPos(glfwWindow, &winPosX, &winPosY);

		if (borderlessFullScreen){
			glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_FALSE);
			glfwMaximizeWindow(glfwWindow);
		}
		else {
			glfwMaximizeWindow(glfwWindow);
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(glfwWindow, monitor, 0, 0, width = videoMode->width, height = videoMode->height, GLFW_DONT_CARE);
		}
	}
	else {
		glfwSetWindowAttrib(glfwWindow, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowMonitor(glfwWindow, nullptr, winPosX, winPosY, width, height, GLFW_DONT_CARE);
		glfwRestoreWindow(glfwWindow);
	}
	renderingSystem->adaptViewport(width, height);
};
void Window::setIcon(std::string path) {
	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(glfwWindow, 1, images);
	stbi_image_free(images[0].pixels);
}
void Window::setWindowCallbacks() {
	glfwSetWindowPosCallback(glfwWindow, whenWindowIsMoved);
	glfwSetFramebufferSizeCallback(glfwWindow, whenWindowIsResized);
	glfwSetScrollCallback(glfwWindow, whenMouseIsScrolling);
	glfwSetKeyCallback(glfwWindow, whenButtonIsPressed);
	glfwSetMouseButtonCallback(glfwWindow, whenMouseIsPressed);
	glfwSetWindowFocusCallback(glfwWindow, whenWindowChangedFocus);
}

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
	parentWindowClass->renderingSystem->adaptViewport(width, height);
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
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {

	}
}