#include "Program.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void centerWindow(Window & window) {
	//figure out center of WORKABLE area
	int x, y, w, h;
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &x, &y, &w, &h);
	int centerOfWorkableAreaX = x + (w / 2);
	int centerofWorkableAreaY = y + (h / 2);
	//figure out top left corner based on center (x,y reused)
	x = centerOfWorkableAreaX - (window.width / 2);
	y = centerofWorkableAreaY - (window.height / 2);
	glfwSetWindowPos(window.glfwWindow, x, y);
}

void reloadTheWindow(Window & window) {
	
	debugPrint("\nWindow is being reloaded!");

	//3. stop rendering thread
	window.keepThreadRunning = false;
	window.thread->join();
	//delete window.thread;

	//2. save current position
	int x, y;
	glfwGetWindowPos(window.glfwWindow, &x, &y);

	//1. destroy window
	GameServer * tmpG = window.renderer->gameServer;
	bool tmpWireframe = window.renderer->wireframeMode;
	destroyGLFWWindow(window);

	//mirror

	//1. create window
	createGLFWWindow(window, *tmpG);
	window.renderer->wireframeMode = tmpWireframe;
	//2. set current position
	glfwSetWindowPos(window.glfwWindow, x, y);

	//3. start rendering thread
	window.keepThreadRunning = true;
	window.thread = std::make_unique<std::thread>(RenderThread, std::ref(window));

}

void toggleFullscreen(Window & window) {
	window.fullScreen = !window.fullScreen;
	if (window.fullScreen) {
		if (window.borderlessFullScreen) {
			glfwSetWindowAttrib(window.glfwWindow, GLFW_DECORATED, GLFW_FALSE);

			glfwMaximizeWindow(window.glfwWindow);
		}
		else {
			glfwMaximizeWindow(window.glfwWindow);
			GLFWmonitor * monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode * videoMode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window.glfwWindow, monitor, 0, 0, window.width = videoMode->width, window.height = videoMode->height, GLFW_DONT_CARE);
		}
	}
	else {
		glfwSetWindowMonitor(window.glfwWindow, nullptr, 0, 0, window.width, window.height, GLFW_DONT_CARE);
		glfwRestoreWindow(window.glfwWindow);
		if (window.decorated) {
			glfwSetWindowAttrib(window.glfwWindow, GLFW_DECORATED, GLFW_TRUE);
		}
	}
}

void destroyGLFWWindow(Window & window) {
	uncaptureCursor(window);
	glfwDestroyWindow(window.glfwWindow);
}

void setWindowCallbacks(Window & window) {
	glfwSetFramebufferSizeCallback(window.glfwWindow, whenWindowIsResized);
	glfwSetScrollCallback(window.glfwWindow, whenMouseIsScrolling);
	glfwSetKeyCallback(window.glfwWindow, whenButtonIsPressed);
	glfwSetMouseButtonCallback(window.glfwWindow, whenMouseIsPressed);
	glfwSetWindowFocusCallback(window.glfwWindow, whenWindowChangedFocus);
	glfwSetCursorPosCallback(window.glfwWindow, whenMouseIsMoving);
}

void whenWindowChangedFocus(GLFWwindow * window, int focused) {

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
		uncaptureCursor(*parentWindowClass);
	}
}

void setIcon(Window & window, std::string path) {
	GLFWimage images[1];
	images[0].pixels = stbi_load(path.c_str(), &images[0].width, &images[0].height, 0, 4);
	glfwSetWindowIcon(window.glfwWindow, 1, images);
	stbi_image_free(images[0].pixels);
}

void createGLFWWindow(Window & window, GameServer & gameServer) {
	const GLFWvidmode * videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, window.antiAliasing);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	//GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
	if(!window.decorated) glfwWindowHint(GLFW_DECORATED, GL_FALSE);
#ifdef DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	try {
		window.glfwWindow = glfwCreateWindow(window.width, window.height, window.title.c_str(), NULL, NULL);
		if (!window.glfwWindow) {
			throw std::exception("Failed to initialize Window!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	glfwSetWindowUserPointer(window.glfwWindow, &window);

	setIcon(window, "icon.png");

	if (glfwRawMouseMotionSupported()) glfwSetInputMode(window.glfwWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);


	window.renderer = std::make_unique<Renderer>(gameServer,window.camera);

	if (window.fullScreen) {
		window.fullScreen = false;
		toggleFullscreen(window);
	}

	glfwShowWindow(window.glfwWindow);

	setWindowCallbacks(window);
}

void changeAntiAliasing(Window & window, unsigned int antiAliasing) {
	window.antiAliasing = antiAliasing;
	reloadTheWindow(window);
}

Window::Window(GameServer & gameServer)
{
	createGLFWWindow(*this, gameServer);
	centerWindow(*this);

	thread = std::make_unique<std::thread>(RenderThread, std::ref(*this));
	debugPrint("|--Window was created!");
}
Window::~Window() {
	keepThreadRunning = false;
	thread->join();

	destroyGLFWWindow(*this);

	debugPrint("|--Window was destroyed!");
}

void RenderThread(Window & window)
{
	glfwMakeContextCurrent(window.glfwWindow);

	glewExperimental = true;
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

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugOutputCallback, 0);
	
	loadShader(window.renderer->primitiveShader, "shaders/primitive.vert", "shaders/primitive.frag");
	loadShader(window.renderer->primitiveShaderInstanced, "shaders/primitive instanced.vert", "shaders/primitive.frag");
	loadShader(window.renderer->primitiveChunk, "shaders/primitive chunk.vert", "shaders/primitive chunk.frag");

	loadAllMeshes(window.renderer->meshSystem);
	
	while (window.keepThreadRunning) {		
		updateTime(window.renderer->renderTime);
		processKeyboardInput(window);
		pocessCamera(window.camera, window.renderer->renderTime);
		renderWindow(window);
	}
}