
#include "Window.hpp"

//window thread
void windowThread(Window& window)
{

	Renderer& renderer = window.renderer;

	window.initializeGraphicsAPI(); //needs to be in this thread
	renderer.create();		//also needs to be in this thread

	window.show();

	if (window.fullScreen) {
		window.setExclusiveFullscreen();
	}
	else {
		window.setWindowed();
	}

	window.setCallbacks();

	//main render loop for the window
	while (window.thread.keepThreadRunning) {
		if (window.isShown) {

			renderer.begin(); //resets and syncronizes the renderer

			TiledRectangle windowArea;
			windowArea.size = window.getWindowContentSize();
			renderer.framebufferSystem.update(windowArea.size);

			renderer.renderRegion.set(windowArea);

			renderer.framebufferSystem.use(0);
			renderer.clearColor(Color(Vec3(0),0.0));
			renderer.clearDepth();

			renderer.setWireframeMode(renderer.wireframeMode);

			window.userInterface.render(window);

			/// ////////////////////////////////////////////////////////////////////////////
			renderer.framebufferSystem.current().blitFramebuffer();
			renderer.end(); //checks errors and unlocks renderer
			apiWindowSwapBuffers(window.apiWindow);
		}
		printDebugLog();
	}

	window.userInterface.destroy(window);

	window.destroyAPIWindow();
}

//window member functions
void Window::show()
{
	apiWindowSetVisibility(apiWindow, false);
}
void Window::hide()
{
	apiWindowSetVisibility(apiWindow, false);
}
void Window::create()
{
	createAPIWindow(); //needs to be in this thread
	centerWindow();

	thread.start(windowThread, *this);
	logEvent("Created Window in Main Thread!");
}
void Window::destroy() {
	thread.stop();
}
void Window::setWindowed()
{
	TiledRectangle windowArea;
	windowArea.size = getWindowContentSize();
	apiWindowSetWindowedMode(apiWindow, windowArea);
	restoreWindow();
	if (decorated) {
		decorateWindow();
	}
	else {
		undecorateWindow();
	}
	centerWindow();
	fullScreen = false;
}
void Window::setCallbacks()
{
	glfwSetCharCallback(apiWindow, whenCharIsTyped);
	glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
	glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
	glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
	glfwSetDropCallback(apiWindow, whenFileDroppedOnWindow);
	glfwSetWindowSizeCallback(apiWindow, whenWindowResized);
	glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
	glfwSetCursorEnterCallback(apiWindow, whenMouseEnterWindow);
	glfwSetWindowFocusCallback(apiWindow, whenWindowChangedFocus);
	glfwSetWindowCloseCallback(apiWindow, whenWindowCloseRequest);
	glfwSetWindowIconifyCallback(apiWindow, whenWindowWasMinimized);
	glfwSetWindowMaximizeCallback(apiWindow, whenWindowWasMaximized);
	glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
	glfwSetWindowRefreshCallback(apiWindow, whenWindowDamagedOrRefreshed);
	glfwSetWindowContentScaleCallback(apiWindow, whenWindowContentScaleChanged);
}
void Window::centerWindow() {
	TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - getWindowFrameSize().center());
}
void Window::restoreWindow()
{
	apiWindowRestore(apiWindow);
}
void Window::decorateWindow()
{
	apiWindowDecorate(apiWindow);
}
void Window::updatePosition()
{
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
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

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
		glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
		//if (!decorated)
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		apiWindow = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, title.c_str(), NULL, NULL);

		assert(apiWindow);

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("Data/textures/ORKA Logo.png");

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		updatePosition();
	}
	else {
		logError("Window already exists!");
	}
}
void Window::undecorateWindow()
{
	apiWindowUndecorate(apiWindow);
}
void Window::destroyAPIWindow() {

	inputManager.uncaptureCursor(*this);
	if (apiWindow) {
		glfwDestroyWindow(apiWindow);
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
void Window::initializeGraphicsAPI() {
	glfwMakeContextCurrent(apiWindow);
	glewExperimental = true;
	if (glewInit() == GLEW_OK) {
		logEvent("GLEW successfully initialized!");
	}
	else {
		logError("GLEW not initialized!");
	}

	glfwSwapInterval(0);

	apiDebugMessageCallback(DebugOutputCallback, 0);
	apiEnable(GL_DEBUG_OUTPUT);
	apiEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
void Window::setExclusiveFullscreen() {

	glfwMaximizeWindow(apiWindow);
	if (borderlessFullScreen) {
		undecorateWindow();
	}
	else {
		glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	}
	//help glfw recognize frambuffer change
	//updateFramebuffers();
	fullScreen = true;
}
void Window::setPosition(IVec2 position)
{
	windowPosition = position;
	updatePosition();
}

//window getters
Bool Window::shouldClose()
{
	return glfwWindowShouldClose(apiWindow);
}
Bool Window::isFullScreen()
{
	return apiWindowIsFullscreen(apiWindow);
}
Area Window::getWindowContentSize()
{
	return apiWindowGetFramebufferSize(apiWindow);
}
Area Window::getWindowFrameSize()
{
	return apiWindowGetWindowFrameSize(apiWindow);
}

//window callbacks
void whenWindowCloseRequest(APIWindow apiWindow) {
	logDebug("Window requested to close");
	//if (!true) glfwSetWindowShouldClose(apiWindow, false);
}
void whenWindowDamagedOrRefreshed(APIWindow apiWindow) {
}
void whenMonitorChanged(APIMonitor monitor, Int event)
{
	if (event == GLFW_CONNECTED)
	{
		// The monitor was connected
	}
	else if (event == GLFW_DISCONNECTED)
	{
		// The monitor was disconnected
	}
}
void whenCharIsTyped(APIWindow apiWindow, UInt character)
{
}
void whenMouseEnterWindow(APIWindow apiWindow, Int entered) {

};
void whenWindowChangedFocus(APIWindow apiWindow, Int focused) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.windowChangedFocus(window, focused);
}
void whenWindowWasMaximized(APIWindow apiWindow, Int maximized)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
}
void whenWindowWasMinimized(APIWindow apiWindow, Int minimized)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
}
void whenWindowResized(APIWindow apiWindow, Int width, Int height)
{
}
void whenFramebufferIsResized(APIWindow apiWindow, Int width, Int height) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	ULL current = window.renderer.frameCount;

	if (!window.isFullScreen()) window.windowedModeSize = Area(width, height);

	if (apiWindowKeyIsPressed(apiWindow, Key::SHIFT)) {
		window.centerWindow();
	}

	while (current == window.renderer.frameCount) {}
}
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	inputManager.mouseWheelIsScrolled(window, xAxis, yAxis);
}
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!inputManager.isCapturing(window)) {
			inputManager.captureCursor(window);
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (inputManager.isCapturing(window)) {
			inputManager.uncaptureCursor(window);
		}
	}
}
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.mouseIsMoving(window, Vec2(xPosition, yPosition));
}
void whenFileDroppedOnWindow(APIWindow apiWindow, Int count, const Char** paths)
{
	for (Int i = 0; i < count; i++) {
		logDebug(String("File dropped: ").append(paths[i]));
	}
}
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale)
{
}
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	inputManager.buttonIsPressed(window, key, action, modifiers);

}