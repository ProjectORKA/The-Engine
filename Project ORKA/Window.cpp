

#include "Window.hpp"

void windowThread(Window& window)
{
	window.initializeGraphicsAPI();

	window.renderLoop();
}

void Window::create()
{
	createAPIWindow();

	center();
}
void Window::center() {
	Rect<Int> workableArea;
	getWorkableArea(workableArea);
	setPosition(workableArea.center() - Vec2(Float(windowContentWidth) / 2, Float(windowContentHeight)/2));
}
void Window::reload() {
#ifdef WINDOW_API_GLFW
	destroyAPIWindow();
	createAPIWindow();
#endif // WINDOW_API_GLFW
}
void Window::destroy() {
	destroyAPIWindow();
}
void Window::pushFrame()
{
#ifdef WINDOW_API_GLFW
	glfwSwapBuffers(apiWindow);
#endif // WINDOW_API_GLFW
}
void Window::renderLoop() {

	renderer.create();

	glfwShowWindow(apiWindow);

	if (fullScreen) {
		setFullscreen();
	}
	else {
		setWindowed();
	}

	while (keepThreadRunning) {
		if (isShown) {

			processCameraMovement();

			renderer.render();

			pushFrame();
		}
	}

	renderer.destroy();
}
void Window::stopThread()
{
	keepThreadRunning = false;
	if (thread.joinable()) {
		thread.join();
	}
	else {
		logError("WindowThread is not joinable!");
	}
}
bool Window::shouldClose()
{
#ifdef WINDOW_API_GLFW
	return glfwWindowShouldClose(apiWindow);
#endif // WINDOW_API_GLFW
}
void Window::startThread()
{
	keepThreadRunning = true;
	thread = Thread(windowThread, std::ref(*this));
}
void Window::setWindowed()
{
	glfwSetWindowMonitor(apiWindow, nullptr, 0, 0, windowContentWidth, windowContentHeight, GLFW_DONT_CARE);
	glfwRestoreWindow(apiWindow);
	if (decorated) {
		glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, GLFW_TRUE);
	}
	center();
	fullScreen = false;
}
void Window::processInput() {
	processCameraMovement();
}
void Window::captureCursor()
{
	if (glfwGetInputMode(apiWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
		glfwGetCursorPos(apiWindow, &cursorPosition.x, &cursorPosition.y);
		glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(apiWindow, 0, 0);
		capturingCursor = true;
	}
}
void Window::setFullscreen() {

	if (borderlessFullScreen) {
		glfwSetWindowAttrib(apiWindow, GLFW_DECORATED, GLFW_FALSE);

		glfwMaximizeWindow(apiWindow);
	}
	else {
		glfwMaximizeWindow(apiWindow);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(apiWindow, monitor, 0, 0, videoMode->width, videoMode->height, GLFW_DONT_CARE);

		//[TODO] check if necessary
		//renderer.framebufferSystem.current().update(width = videoMode->width;
		//renderer.framebufferSystem.current().height = videoMode->height;
	}
	fullScreen = true;
}
void Window::updatePosition()
{
#ifdef WINDOW_API_GLFW
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
#endif // WINDOW_API_GLFW
}
void Window::createAPIWindow() {
	if (!apiWindow) {
		//video mode
		const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_DEPTH_BITS, 32);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		//visibility
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		//opengl stuff
		glfwWindowHint(GLFW_SAMPLES, antiAliasing);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	//GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
		if (!decorated) glfwWindowHint(GLFW_DECORATED, GL_FALSE);

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		apiWindow = glfwCreateWindow(windowContentWidth, windowContentHeight, title.c_str(), NULL, NULL);

		assert(apiWindow);

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("icon.png");

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		updatePosition();

		startThread();

		//set window callbacks
		glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
		glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
		glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
		glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
		glfwSetWindowPosCallback(apiWindow, whenWindowIsBeingMoved);
		glfwSetWindowFocusCallback(apiWindow, whenWindowChangedFocus);
		glfwSetWindowIconifyCallback(apiWindow, whenWindowWasMinimized);
		glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
	}
	else {
		logError("Window already exists!");
	}
}
void Window::uncaptureCursor() {
	if (glfwGetInputMode(apiWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(apiWindow, cursorPosition.x, cursorPosition.y);
		capturingCursor = false;
	}
}
void Window::destroyAPIWindow() {

	stopThread();

	uncaptureCursor();

	if (apiWindow) {

#ifdef WINDOW_API_GLFW
		glfwDestroyWindow(apiWindow);
#endif // WINDOW_API_GLFW
		apiWindow = nullptr;
	}
	logEvent("API Window destroyed!");
}
void Window::setIcon(Path path) {

	CPUTexture cpuTexture;
	invertOnLoad(false);
	cpuTexture.load(path, "Icon");
	invertOnLoad(true);

	if (cpuTexture.bytePixels && cpuTexture.dataType == dataTypeByte) {
		GLFWimage icon;
		icon.pixels = cpuTexture.bytePixels;
		icon.width = cpuTexture.width;
		icon.height = cpuTexture.height;
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
}
void Window::processCameraMovement() {

	OctreeWorldSystemCamera& camera = renderer.cameraSystem.current();

	if (glfwGetKey(apiWindow, GLFW_KEY_E) == GLFW_PRESS) camera.accelerationVector += camera.upVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_Q) == GLFW_PRESS) camera.accelerationVector -= camera.upVector;

	if (glfwGetKey(apiWindow, GLFW_KEY_W) == GLFW_PRESS) camera.accelerationVector += camera.forwardVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_S) == GLFW_PRESS) camera.accelerationVector -= camera.forwardVector;

	if (glfwGetKey(apiWindow, GLFW_KEY_D) == GLFW_PRESS) camera.accelerationVector += camera.rightVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_A) == GLFW_PRESS) camera.accelerationVector -= camera.rightVector;
}
void Window::initializeGraphicsAPI() {
#ifdef GRAPHICS_API_OPENGL
#ifdef WINDOW_API_GLFW
	glfwMakeContextCurrent(apiWindow);
#endif // WINDOW_API_GLFW
	glewExperimental = true;
	if (glewInit() == GLEW_OK) {
		logDebug("GLEW successfully initialized!");
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugOutputCallback, 0);
#endif // GRAPHICS_API_OPENGL
}
void Window::setPosition(Vec2 position)
{
	windowPosition = position;
	updatePosition();
}
void Window::getWorkableArea(Rect<Int>& rect)
{
#ifdef WINDOW_API_GLFW
	glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &rect.x, &rect.y, &rect.width, &rect.height);
#endif // WINDOW_API_GLFW
}
void Window::changeAntiAliasing(UShort antiAliasing) {
	this->antiAliasing = antiAliasing;
	reload();
}

void whenWindowChangedFocus(APIWindow window, Int focused) {

	Window* parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (focused) {
		if (parentWindowClass->capturingCursor) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	else {
		parentWindowClass->uncaptureCursor();
	}
}
void whenWindowWasMinimized(APIWindow window, Int minimized)
{
	//[TODO] possible optimization
	//- change framebuffer to 1 x 1

	logEvent("Window minimized!");
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (minimized)
	{
		parentWindowClass.isShown = false;
	}
	else
	{
		parentWindowClass.isShown = true;
	}
}
void whenWindowIsBeingMoved(APIWindow window, int xpos, int ypos) {
	Window* parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));

	parentWindowClass->windowPosition = Vec2(xpos, ypos);
};
void whenMouseIsMoving(APIWindow apiWindow, Double xpos, Double ypos) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	if (window.apiWindow) {
		if (window.capturingCursor) {
			window.deltaX = xpos;
			window.deltaY = ypos;
			glfwSetCursorPos(window.apiWindow, 0, 0);
			if(window.renderer.cameraSystem.cameras.size() > 0) window.renderer.cameraSystem.current().rotate(xpos, ypos);	
		}
		else {
			window.cursorPosition.x = xpos;
			window.cursorPosition.y = ypos;
	}
	}
}
void whenFramebufferIsResized(APIWindow window, Int width, Int height) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));

	parentWindowClass.windowContentWidth = width;
	parentWindowClass.windowContentHeight = height;
	parentWindowClass.renderer.framebufferSystem.adaptiveResolution = Vec2(width, height);

	parentWindowClass.renderer.sync();

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		parentWindowClass.center();
	}
}
void whenMouseIsScrolling(APIWindow window, Double xoffset, Double yoffset) {
	static_cast<Window*>(glfwGetWindowUserPointer(window))->renderer.cameraSystem.current().speedMultiplier += yoffset;
}
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!parentWindowClass.capturingCursor) {
			parentWindowClass.captureCursor();
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass.capturingCursor) {
			parentWindowClass.uncaptureCursor();
		}
	}
}
void whenButtonIsPressed(APIWindow window, Int key, Int scancode, Int action, Int mods)
{
	Window* parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		if (parentWindowClass->fullScreen) parentWindowClass->setWindowed();
		else parentWindowClass->setFullscreen();
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(parentWindowClass->apiWindow, GLFW_TRUE);
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(0);
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		parentWindowClass->changeAntiAliasing(4);
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		parentWindowClass->renderer.chunkBorders = !parentWindowClass->renderer.chunkBorders;
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		parentWindowClass->renderer.gameSimulation->gameTime.paused = !parentWindowClass->renderer.gameSimulation->gameTime.paused;
	}
	if (key == GLFW_KEY_N && action == GLFW_PRESS) {
		parentWindowClass->duplicateWindow = true;
	}
	if (key == GLFW_KEY_B && action == GLFW_PRESS) {
		parentWindowClass->borderlessFullScreen = !parentWindowClass->borderlessFullScreen;
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		parentWindowClass->renderer.wireframeMode = !parentWindowClass->renderer.wireframeMode;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		parentWindowClass->renderer.adjustRenderVariables = !parentWindowClass->renderer.adjustRenderVariables;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS) {
		parentWindowClass->renderer.worldDistortion = !parentWindowClass->renderer.worldDistortion;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		parentWindowClass->renderer.chunkBorders = !parentWindowClass->renderer.chunkBorders;
	}
}

#ifdef GRAPHICS_API_OPENGL
void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
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

	}
}
#endif // GRAPHICS_API_OPENGL