
#include "Window.hpp"

void windowThread(Window& window)
{
	window.initializeGraphicsAPI(); //needs to be in this thread

	window.renderer.create();
	window.updateFramebuffers();

	glfwShowWindow(window.apiWindow);
	if (window.fullScreen) {
		window.setFullscreen();
	}
	else {
		window.setWindowed();
	}

	while (window.thread.keepThreadRunning) {
		if (window.isShown) {

			window.processCameraMovement();

			window.renderer.begin();

			window.layer.render();

			window.renderer.renderFramebuffer();

			window.renderer.end();

			window.pushFrame();
		}
	}
}

void Window::create()
{
	createAPIWindow();
	center();
	thread.start(windowThread, *this);
}
void Window::center() {
	Rect<Int> workableArea;
	getWorkableArea(workableArea);
	setPosition(workableArea.center() - Vec2(Float(windowContentWidth) / 2, Float(windowContentHeight)/2));
}
void Window::destroy() {
	thread.stop();
	renderer.destroy();
	destroyAPIWindow();
}
void Window::pushFrame()
{
#ifdef WINDOW_API_GLFW
	glfwSwapBuffers(apiWindow);
#endif // WINDOW_API_GLFW
}
bool Window::shouldClose()
{
#ifdef WINDOW_API_GLFW
	return glfwWindowShouldClose(apiWindow);
#endif // WINDOW_API_GLFW
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

	updateFramebuffers();
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
		isCapturingCursor = true;
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
	}
	//help glfw recognize frambuffer change
	updateFramebuffers();
	fullScreen = true;
}
void Window::updatePosition()
{
#ifdef WINDOW_API_GLFW
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
#endif // WINDOW_API_GLFW
}
void Window::uncaptureCursor() {
	if (glfwGetInputMode(apiWindow, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		glfwSetInputMode(apiWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(apiWindow, cursorPosition.x, cursorPosition.y);
		isCapturingCursor = false;
	}
}
void Window::createAPIWindow() {
	if (!apiWindow) {
		//video mode
		const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		//visibility
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		//opengl stuff
		glfwWindowHint(GLFW_SAMPLES, 0);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
		if (!decorated) glfwWindowHint(GLFW_DECORATED, GL_FALSE);

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		apiWindow = glfwCreateWindow(windowContentWidth, windowContentHeight, title.c_str(), NULL, NULL);

		assert(apiWindow);

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("Data/textures/ORKA Logo.png");

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		updatePosition();

		//set window callbacks
		glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
		glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
		glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
		glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
		glfwSetWindowPosCallback(apiWindow, whenWindowIsBeingMoved);
		glfwSetWindowFocusCallback(apiWindow, whenWindowChangedFocus);
		glfwSetWindowIconifyCallback(apiWindow, whenWindowWasMinimized);
		glfwSetWindowMaximizeCallback(apiWindow, whenWindowWasMaximized);
		glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
	}
	else {
		logError("Window already exists!");
	}
}
void Window::destroyAPIWindow() {

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
	setInvertOnLoad(false);
	cpuTexture.load(path, "Icon");
	setInvertOnLoad(true);

	if (cpuTexture.bytePixels && cpuTexture.dataType == dataTypeByte) {
		GLFWimage icon;
		icon.pixels = cpuTexture.bytePixels;
		icon.width = cpuTexture.width;
		icon.height = cpuTexture.height;
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
}
void Window::updateFramebuffers()
{
	Int width;
	Int height;
	glfwGetFramebufferSize(apiWindow, &width, &height);
	renderer.framebufferSystem.updateSizes(width, height);
}
void Window::processCameraMovement() {

	PlanetCamera& camera = renderer.cameraSystem.currentPlanetCamera();

	if (glfwGetKey(apiWindow, GLFW_KEY_E) == GLFW_PRESS) camera.accelerationVector += camera.upVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_Q) == GLFW_PRESS) camera.accelerationVector -= camera.upVector;

	if (glfwGetKey(apiWindow, GLFW_KEY_W) == GLFW_PRESS) camera.accelerationVector += camera.forwardVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_S) == GLFW_PRESS) camera.accelerationVector -= camera.forwardVector;

	if (glfwGetKey(apiWindow, GLFW_KEY_D) == GLFW_PRESS) camera.accelerationVector += camera.rightVector;
	if (glfwGetKey(apiWindow, GLFW_KEY_A) == GLFW_PRESS) camera.accelerationVector -= camera.rightVector;
}
void Window::initializeGraphicsAPI() {

#ifdef WINDOW_API_GLFW
	glfwMakeContextCurrent(apiWindow);
#endif // WINDOW_API_GLFW
	glewExperimental = true;
	if (glewInit() == GLEW_OK) {
		logEvent("GLEW successfully initialized!");
	}
	else {
		logError("GLEW not initialized!");
	}

	glfwSwapInterval(0);

	glDebugMessageCallback(DebugOutputCallback, 0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
	logDebug(String("Changing AA Samples (").append(std::to_string(renderer.multisampleCount)).append(")."));
	renderer.multisampleCount = antiAliasing;
}

void whenWindowChangedFocus(APIWindow window, Int focused) {

	Window* parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (focused) {
		if (parentWindowClass->isCapturingCursor) {
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
void whenWindowWasMaximized(APIWindow window, Int maximized)
{
	//Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));
	//Int width;
	//Int height;
	//if (maximized)
	//{
	//	glfwGetFramebufferSize(window, &width, &height);
	//	logDebug(width);
	//	parentWindowClass.renderer.framebufferSystem.updateFramebufferSizes(width,height);
	//}
	//else
	//{
	//	parentWindowClass.isShown = true;
	//}
}
void whenWindowWasMinimized(APIWindow window, Int minimized)
{
	//Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(window));
	//if (minimized)
	//{
	//	parentWindowClass.isShown = false;
	//}
	//else
	//{
	//	parentWindowClass.isShown = true;
	//}
}
void whenWindowIsBeingMoved(APIWindow window, int xpos, int ypos) {
	Window* parentWindowClass = static_cast<Window*>(glfwGetWindowUserPointer(window));

	parentWindowClass->windowPosition = Vec2(xpos, ypos);
};
void whenMouseIsMoving(APIWindow apiWindow, Double xpos, Double ypos) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	if (window.apiWindow) {
		if (window.isCapturingCursor) {
			window.deltaX = xpos;
			window.deltaY = ypos;
			glfwSetCursorPos(window.apiWindow, 0, 0);
			window.renderer.cameraSystem.currentPlanetCamera().rotate(xpos * window.inputSystem.mouseSensitivity, ypos * window.inputSystem.mouseSensitivity);
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
	parentWindowClass.updateFramebuffers();
	parentWindowClass.renderer.waitForFinishedFrame();

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		parentWindowClass.center();
	}
}
void whenMouseIsScrolling(APIWindow window, Double xoffset, Double yoffset) {
	static_cast<Window*>(glfwGetWindowUserPointer(window))->renderer.cameraSystem.currentPlanetCamera().speedMultiplier += yoffset;
}
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!parentWindowClass.isCapturingCursor) {
			parentWindowClass.captureCursor();
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (parentWindowClass.isCapturingCursor) {
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
		parentWindowClass->changeAntiAliasing(8);
	}
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		//Message message;
		//message.type = 1;
		//message.byteSize = 0;
		//client.sendMessage(message);
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
		parentWindowClass->renderer.planetRenderSystem.worldDistortion = !parentWindowClass->renderer.planetRenderSystem.worldDistortion;
	}
	if (key == GLFW_KEY_G && action == GLFW_PRESS) {
		parentWindowClass->renderer.planetRenderSystem.chunkBorders = !parentWindowClass->renderer.planetRenderSystem.chunkBorders;
	}
}