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
	GameServer * tmp = window.renderer->gameServer;
	destroyGLFWWindow(window);

	//mirror

	//1. create window
	createGLFWWindow(window, *tmp);

	//2. set current position
	glfwSetWindowPos(window.glfwWindow, x, y);

	//3. start rendering thread
	window.keepThreadRunning = true;
	window.thread = std::make_unique<std::thread>(WindowThread, std::ref(window), tmp);

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


	window.renderer = std::make_unique<Renderer>(gameServer);

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

	thread = std::make_unique<std::thread>(WindowThread, std::ref(*this), &gameServer);
	debugPrint("|--Window was created!");
}
Window::~Window() {
	keepThreadRunning = false;
	thread->join();

	destroyGLFWWindow(*this);
	debugPrint("|--Window was destroyed!");
}

void WindowThread(Window & window, GameServer * gameServer)
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

	{
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/tree.fbx", "tree");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/cube.fbx", "cube");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/suzanne.fbx", "monkey");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/icosphere.fbx", "icosphere");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/triangle.fbx", "triangle");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/plane.fbx", "plane");
		createAndUploadMeshFromFile(window.renderer->meshSystem, GL_TRIANGLES, "objects/triangle.fbx", "error");

		//create hardcoded bounding box
		MeshContainer boundingBox;
		boundingBox.name = "bounds";
		boundingBox.primitiveMode = GL_LINES;
		boundingBox.vertices.push_back(glm::vec3(+1.0f, +1.0f, +1.0f));
		boundingBox.vertices.push_back(glm::vec3(+1.0f, +1.0f, -1.0f));
		boundingBox.vertices.push_back(glm::vec3(+1.0f, -1.0f, +1.0f));
		boundingBox.vertices.push_back(glm::vec3(+1.0f, -1.0f, -1.0f));
		boundingBox.vertices.push_back(glm::vec3(-1.0f, +1.0f, +1.0f));
		boundingBox.vertices.push_back(glm::vec3(-1.0f, +1.0f, -1.0f));
		boundingBox.vertices.push_back(glm::vec3(-1.0f, -1.0f, +1.0f));
		boundingBox.vertices.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
		boundingBox.indices.push_back(7);
		boundingBox.indices.push_back(3);
		boundingBox.indices.push_back(7);
		boundingBox.indices.push_back(5);
		boundingBox.indices.push_back(7);
		boundingBox.indices.push_back(6);
		boundingBox.indices.push_back(1);
		boundingBox.indices.push_back(3);
		boundingBox.indices.push_back(1);
		boundingBox.indices.push_back(5);
		boundingBox.indices.push_back(1);
		boundingBox.indices.push_back(0);
		boundingBox.indices.push_back(2);
		boundingBox.indices.push_back(6);
		boundingBox.indices.push_back(2);
		boundingBox.indices.push_back(0);
		boundingBox.indices.push_back(2);
		boundingBox.indices.push_back(3);
		boundingBox.indices.push_back(4);
		boundingBox.indices.push_back(6);
		boundingBox.indices.push_back(4);
		boundingBox.indices.push_back(5);
		boundingBox.indices.push_back(4);
		boundingBox.indices.push_back(0);
		uploadStaticMesh(window.renderer->meshSystem, boundingBox);

		//create hardcoded point mesh
		MeshContainer singlePoint;
		singlePoint.name = "point";
		singlePoint.primitiveMode = GL_POINTS;
		singlePoint.vertices.push_back(glm::vec3(0.0f));
		singlePoint.uvs.push_back(glm::vec2(0.5f));
		singlePoint.indices.push_back(0);
		uploadStaticMesh(window.renderer->meshSystem, singlePoint);

		////create hardcoded point mesh
		//createAndUploadMeshFromFile(window.renderer->meshSystem, GL_POINTS, "objects/detailed monkey.fbx", "point cloud");
	}

	//std::chrono::steady_clock::time_point t;

	while (window.keepThreadRunning) {
		//t = std::chrono::steady_clock::now();
		
		updateTime(window.renderer->renderTime);
		processKeyboardInput(window);
		pocessCamera(window.camera);
		renderWindow(window);

		//t += std::chrono::milliseconds(16); //game server running at 60 Hz
		//std::this_thread::sleep_until(t);
	}
}