
#include "Window.hpp"
#include "InputManager.hpp"
#include "UserInterface.hpp"

UShort nextWindowID = 0;

//window thread
void windowThread(Window& window)
{
	Renderer& renderer = window.renderer;

	window.initializeGraphicsAPI(); //needs to be in this thread
	renderer.create(window.getWindowContentSize());		//also needs to be in this thread


	window.updateWindowState();

	if(window.windowState == Window::windowed) window.centerWindow();
	if (window.windowState == Window::windowed || window.windowState == Window::maximized) window.updateDecorations();


	window.setCallbacks();

	//main render loop for the window
	while (window.thread.keepThreadRunning) {
		if (window.isShown) {
			renderer.begin(); //resets and syncronizes the renderer
			renderer.framebufferSystem.update(window.getWindowContentSize());
			renderer.renderRegion.set(window.getWindowContentSize());

			//gpu needs frameSize
			renderer.uniforms().width(renderer.framebufferSystem.current().size.x);
			renderer.uniforms().height(renderer.framebufferSystem.current().size.y);

			//render interactibles
			renderer.framebufferSystem.idFramebuffer.use();
			renderer.clearColor(Color(Vec3(0), 0));
			renderer.clearDepth();

			//render scene
			renderer.framebufferSystem.use(renderer, 0);
			renderer.clearColor(Color(Vec3(0), 0.0));
			renderer.clearDepth();


			renderer.setWireframeMode(renderer.wireframeMode);

			if (window.content) window.content->render(window.renderer);

			renderer.setWireframeMode(false);
			renderer.setAlphaBlending(false);

			renderer.framebufferSystem.current().blitFramebuffer();

			apiBindDrawFramebuffer(0);

			renderer.useShader("final");
			renderer.framebufferSystem.current().colorTexture.use(0);
			renderer.uniforms().reset();
			renderer.renderMesh("plane");

			renderer.end(); //checks errors and unlocks renderer
			apiWindowSwapBuffers(window.apiWindow);
		}
		printDebugLog();
	}

	window.destroyAPIWindow();
}

//window member functions
void Window::render() {
	if (content)content->render(renderer);
}
void Window::destroy() {
	thread.stop();
}
void Window::setWindowed()
{
	apiWindowSetWindowedMode(apiWindow, windowedModeSize);
	apiWindowRestore(apiWindow);
	centerWindow();
	windowState = windowed;
}
void Window::setMaximized() {
	apiMaximizeWindow(apiWindow);
	windowState = maximized;
}
void Window::setCallbacks()
{
	glfwSetCharCallback(apiWindow, whenCharIsTyped);
	glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
	glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
	glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
	//glfwSetWindowSizeCallback(apiWindow, whenWindowResized);
	glfwSetDropCallback(apiWindow, whenFilesDroppedOnWindow);
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
void Window::setMinimized() {
	apiMinimizeWindow(apiWindow);
	windowState = minimized;
}
void Window::setFullscreen() {
	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	windowState = fullscreen;
}
void Window::decorateWindow()
{
	apiWindowDecorate(apiWindow);
	decorated = true;
}
void Window::updatePosition()
{
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
}
void Window::undecorateWindow()
{
	apiWindowUndecorate(apiWindow);
	decorated = false;
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

	Image logo = loadImage(path, 8, false);

	if (logo.pixels && logo.channels == 4) {
		GLFWimage icon;
		icon.pixels = logo.pixels;
		icon.width = logo.width;
		icon.height = logo.height;
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else {
		logError("Logo could not be Loaded!");
	}
}
void Window::updateWindowState() {
	switch (windowState) {
	case fullscreen: setFullscreen(); break;
	case maximized: setMaximized(); break;
	case windowed: setWindowed(); break;
	case minimized: setMinimized(); break;
	default: logError("Window state not supported!"); break;
	}
}
void Window::updateDecorations() {
	if (decorated) decorateWindow();
	else undecorateWindow();
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
void Window::setPosition(IVec2 position)
{
	windowPosition = position;
	updatePosition();
}
void Window::resize(Int width, Int height) {
	apiWindowResize(apiWindow, width, height);
}
void Window::createAPIWindow(String title, Area size) {
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

		apiWindow = apiCreateWindow(size, title.c_str());

		if (!apiWindow)logError("Window could not be created!");

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("Data/textures/ORKA Logo.png");

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else {
		logError("Window already exists!");
	}
}
void Window::create(String title, UIElement* content, Area size, Bool decorated, WindowState state)
{
	this->decorated = decorated;
	this->windowState = state;

	id = nextWindowID++;
	windowedModeSize = size;
	createAPIWindow(title, size); //needs to be in this thread

	if (content)this->content = content;
	ui.currentlyActive = content;

	thread.start(windowThread, *this);
	logEvent("Created Window in Main Thread!");
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
Area Window::getWindowContentSize()
{
	return apiWindowGetFramebufferSize(apiWindow);
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
	inputManager.mouseIsPressed(window, button, action, mods);
}
void whenMouseIsMoving(APIWindow apiWindow, Double xPosition, Double yPosition) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.mouseIsMoving(window, Vec2(xPosition, yPosition));
}
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale)
{
}
void whenFilesDroppedOnWindow(APIWindow apiWindow, Int count, const Char** droppedPaths)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	Vector<Path> paths;

	for (Int i = 0; i < count; i++) {
		paths.push_back(droppedPaths[i]);
		logDebug(String("File dropped: ").append(paths.back().string()));
	}
	inputManager.filesDropped(window, paths);
}
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	inputManager.buttonIsPressed(window, key, action, modifiers);
}