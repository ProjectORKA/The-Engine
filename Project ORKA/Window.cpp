
#include "Window.hpp"

//window thread
void windowThread(Window& window)
{
	logEvent("Started Window Thread!");
	window.initializeGraphicsAPI(); //needs to be in this thread
	window.renderer.create();		//also needs to be in this thread

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

			window.renderer.begin(); //resets and syncronizes the renderer

			TiledRectangle windowArea;
			windowArea.size = window.getFramebufferSize();

			window.renderer.framebufferSystem.updateSizes(windowArea.size);

			//for (UIElement* element : window.contents) {
			//	element->render(window, windowArea);
			//}

			window.contents[0]->render(window, windowArea);

			Float topbarHeight = 100;

			TiledRectangle topBar = windowArea;
			topBar.size.y = topbarHeight;
			topBar.position.y = windowArea.size.y - topbarHeight;

			window.contents[1]->render(window, topBar);

			window.renderer.renderRegion.setRenderRegion(windowArea);

			window.renderer.framebufferSystem.current().setAsTexture();
			window.renderer.framebufferSystem.deselect();

			window.renderer.clearColor(Color(0, 0, 0, 0.75));

			window.renderer.setWireframeMode(false);
			window.renderer.shaderSystem.use("postProcess");
			window.renderer.meshSystem.renderMesh("fullScreenQuad");

			window.renderer.end(); //checks errors and unlocks renderer

			apiWindowSwapBuffers(window.apiWindow);
		}
	}

	for (UIElement* element : window.contents) {
		element->destroy(window);
		delete element;
	}
	window.contents.clear();

	window.destroyAPIWindow();
}

//window member functions
void Window::destroy() {
	thread.stop();
}
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
	
	framebufferSize = apiWindowGetFramebufferSize(apiWindow);
	centerWindow();

	contents.push_back(new UIORKAGame());
	contents.push_back(new UIFlatPanel());

	for (UIElement* element : contents) {
		element->create(*this);
	}

	contents.back()->create(*this);

	thread.start(windowThread, *this);
	logEvent("Created Window in Main Thread!");
}
void Window::setWindowed()
{
	TiledRectangle windowArea;
	windowArea.size = getFramebufferSize();
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
	glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
	glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
	glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
	glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
	glfwSetWindowFocusCallback(apiWindow, whenWindowChangedFocus);
	glfwSetWindowIconifyCallback(apiWindow, whenWindowWasMinimized);
	glfwSetWindowMaximizeCallback(apiWindow, whenWindowWasMaximized);
	glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
}
void Window::centerWindow() {
	TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - center(getWindowFrameSize()));
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

		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
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

	glDebugMessageCallback(DebugOutputCallback, 0);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
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
Area Window::getWindowFrameSize()
{
	return apiWindowGetWindowFrameSize(apiWindow);
}
Area Window::getFramebufferSize()
{
	return framebufferSize;
}

//window callbacks
void whenWindowChangedFocus(APIWindow apiWindow, Int focused) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.windowChangedFocus(parentWindowClass,focused);
}
void whenWindowWasMaximized(APIWindow apiWindow, Int maximized)
{
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
}
void whenWindowWasMinimized(APIWindow apiWindow, Int minimized)
{
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
}
void whenFramebufferIsResized(APIWindow apiWindow, Int width, Int height) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	ULL current = parentWindowClass.renderer.frameCount;

	parentWindowClass.framebufferSize.x = width;
	parentWindowClass.framebufferSize.y = height;

	if (apiWindowKeyIsPressed(apiWindow, KeySHIFT)) {
		parentWindowClass.centerWindow();
	}

	while (current == parentWindowClass.renderer.frameCount) {
		sleep(1);
	};

}
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	inputManager.mouseWheelIsScrolled(parentWindowClass, xAxis, yAxis);
}
void whenMouseIsPressed(APIWindow apiWindow, Int button, Int action, Int mods) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	//LMB
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!inputManager.isCapturing(parentWindowClass)) {
			inputManager.captureCursor(parentWindowClass);
		}
	}

	//RMB
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (inputManager.isCapturing(parentWindowClass)) {
			inputManager.uncaptureCursor(parentWindowClass);
		}
	}
}
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition) {
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.mouseIsMoving(parentWindowClass,Vec2(xPosition, yPosition));
}
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers)
{
	Window& parentWindowClass = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	inputManager.buttonIsPressed(parentWindowClass,key, action, modifiers);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && modifiers == GLFW_MOD_ALT) {
		if (parentWindowClass.isFullScreen()) parentWindowClass.setWindowed();
		else parentWindowClass.setExclusiveFullscreen();
	}
}