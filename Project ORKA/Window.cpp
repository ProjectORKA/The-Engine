
#include "Window.hpp"
#include "UserInterface.hpp"
#include "Profiler.hpp"

UShort nextWindowID = 0;

void finish() {
	OPTICK_EVENT();
	glFinish();
}

//window thread
void windowThread(ResourceManager& resourceManager, Window& window)
{
	Renderer& renderer = window.renderer;
	window.initializeGraphicsAPI();					//needs to be in this thread
	renderer.create(resourceManager, window.getContentSize());		//also needs to be in this thread
	window.updateWindowState();

	if (window.content)window.content->create(resourceManager, window);

	if (window.windowState == WindowState::windowed) window.centerWindow();
	if (window.windowState == WindowState::windowed || window.windowState == WindowState::maximized) window.updateDecorations();

	MouseMovement prevPos = MouseMovement(0);

	OPTICK_THREAD("Window Thread");

	//main render loop for the window
	while (window.keeprunning) {
		OPTICK_FRAME("Window Frame");
		if (window.isShown) {

			OPTICK_PUSH("Setup");

			window.mouseDelta = window.mousePosBotLeft - prevPos;
			prevPos = window.mousePosBotLeft;

			renderer.begin(); //resets and syncronizes the renderer

			Area windowSize = window.getContentSize();

			TiledRectangle windowArea(windowSize);
			renderer.framebufferSystem.update(windowSize);

			//gpu needs window size
			renderer.uniforms().windowWidth(windowSize.x);
			renderer.uniforms().windowHeight(windowSize.y);

			OPTICK_POP();

			if (window.content) {
				//update ui
				window.content->update(window);

				//render interactibles
				OPTICK_PUSH("Interactive Draw");
				renderer.idFramebuffer.draw(renderer);
				renderer.clearColor();
				renderer.useShader(resourceManager, "idShader");
				renderer.uniforms().reset();
				renderer.renderMesh(resourceManager, "fullScreenQuad");
				renderer.clearDepth();
				renderer.setWireframeMode(false);
				renderer.setCulling(false);
				renderer.useShader(resourceManager, "idShader");
				renderer.setAlphaBlending(false);
				window.content->renderInteractive(resourceManager, window, windowArea);
				renderer.idFramebuffer.updateIdsUnderCursor(window);
				OPTICK_POP();

				//render ui
				OPTICK_PUSH("Draw");
				renderer.draw("main");
				renderer.clearColor();
				window.content->render(resourceManager, window, windowArea);

				renderer.aspectCorrectNormalizedSpace();

				if (window.profiling) renderer.textRenderSystem.render(resourceManager, renderer, "[R]", Vec2(0.9), FontStyle(0.02));
				OPTICK_POP();
			}

			OPTICK_PUSH("Finalize");

			renderer.setWireframeMode(false);
			renderer.setAlphaBlending(false);

			renderer.renderRegion.set(windowSize);

			renderer.framebufferSystem.drawToWindow();

#ifdef DEBUG_ID_BUFFER
			renderer.framebufferSystem.framebuffers[renderer.idFramebuffer.framebufferID].setAsTexture(0);
			renderer.fullScreenShader(resourceManager, "debugID");
#else
			OPTICK_PUSH("Draw final quad");
			renderer.uniforms().reset();
			renderer.framebufferSystem.currentDraw().setAsTexture(0); //[TODO] might not work; check
			renderer.fullScreenShader(resourceManager, "final");
			OPTICK_POP();
#endif
			renderer.end(); //checks errors and unlocks renderer

			finish();

			apiWindowSwapBuffers(window.apiWindow);
			OPTICK_POP();
		}
		window.mouseDelta = MouseMovement(0);
	}
	
	renderer.destroy();
}

//windowstate
void Window::captureCursor() {
	if (!capturing) {
		mousePos = apiWindowGetCursorPosition(apiWindow);
		apiWindowDisableCursor(apiWindow);
		capturing = true;
	}
}
void Window::uncaptureCursor() {
	if (capturing) {
		apiWindowEnableCursor(apiWindow);
		apiWindowSetCursorPosition(apiWindow, mousePos);
		capturing = false;
	}
}

//window member functions
void Window::destroy() {
	keeprunning = false;
	thread.stop();
	destroyAPIWindow();
	LifetimeGuard::destroy();
}
void Window::setWindowed(){
	checkLifetime();

	apiWindowSetWindowedMode(apiWindow, windowedModeSize);
	apiWindowRestore(apiWindow);
	centerWindow();
	windowState = WindowState::windowed;
}
void Window::setMaximized() {
	checkLifetime();

	apiMaximizeWindow(apiWindow);
	windowState = WindowState::maximized;
}
void Window::setCallbacks()
{
	checkLifetime();

	//glfwSetCharCallback(apiWindow, whenCharIsTyped);
	glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
	glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
	glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
	//glfwSetWindowSizeCallback(apiWindow, whenWindowResized);
	glfwSetDropCallback(apiWindow, whenFilesDroppedOnWindow);
	glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
	//glfwSetCursorEnterCallback(apiWindow, whenMouseEnterWindow);
	//glfwSetWindowFocusCallback(apiWindow, whenWindowChangedFocus);
	glfwSetWindowCloseCallback(apiWindow, whenWindowCloseRequest);
	glfwSetWindowIconifyCallback(apiWindow, whenWindowWasMinimized);
	glfwSetWindowMaximizeCallback(apiWindow, whenWindowWasMaximized);
	glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
	//glfwSetWindowRefreshCallback(apiWindow, whenWindowDamagedOrRefreshed);
	glfwSetWindowContentScaleCallback(apiWindow, whenWindowContentScaleChanged);
}
void Window::centerWindow() {
	checkLifetime();

	TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - getFrameSize().center());
}
void Window::setMinimized() {
	checkLifetime();

	apiMinimizeWindow(apiWindow);
	windowState = WindowState::minimized;
}
void Window::setFullscreen() {
	checkLifetime();

	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	windowState = WindowState::fullscreen;
}
void Window::decorateWindow()
{
	checkLifetime();

	apiWindowDecorate(apiWindow);
	decorated = true;
}
void Window::updatePosition()
{
	checkLifetime();

	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
}
void Window::undecorateWindow()
{
	checkLifetime();

	apiWindowUndecorate(apiWindow);
	decorated = false;
}
void Window::destroyAPIWindow() {
	checkLifetime();

	uncaptureCursor();
	if (apiWindow) {
		glfwDestroyWindow(apiWindow);
		apiWindow = nullptr;
	}
	logEvent("API Window destroyed!");
}
void Window::setIcon(Path path) {
	
	checkLifetime();

	Image logo;
	logo.load(makeAbsolute(path), false);

	if (logo.pixels.size() && logo.channels == 4) {
		GLFWimage icon;
		icon.pixels = logo.pixels.data();
		icon.width = logo.width;
		icon.height = logo.height;
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else {
		logWarning("Logo could not be Loaded!");
	}
}
void Window::updateWindowState() {
	checkLifetime();

	switch (windowState) {
	case WindowState::fullscreen: setFullscreen(); break;
	case WindowState::maximized: setMaximized(); break;
	case WindowState::windowed: setWindowed(); break;
	case WindowState::minimized: setMinimized(); break;
	default: logError("Window state not supported!"); break;
	}
}
void Window::updateDecorations() {
	checkLifetime();

	if (decorated) decorateWindow();
	else undecorateWindow();
}
void Window::initializeGraphicsAPI() {
	checkLifetime();

	glfwMakeContextCurrent(apiWindow);

	glewExperimental = true;

	if (glewInit() == GLEW_OK) {
		logEvent("GLEW successfully initialized!");
	}
	else {
		logError("GLEW not initialized!");
	}

	glfwSwapInterval(0);

	apiSetDebugging(true);
}
void Window::setPosition(IVec2 position)
{
	checkLifetime();

	windowPosition = position;
	updatePosition();
}
Window& Window::insert(UIElement& element) {
	checkLifetime();

	content = &element;
	return *this;
}
void Window::resize(Int width, Int height) {
	checkLifetime();

	apiWindowResize(apiWindow, width, height);
}
void Window::createAPIWindow(String title, Area size) {
	checkLifetime();

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
		if (!decorated) glfwWindowHint(GLFW_DECORATED, GL_FALSE);
		else glfwWindowHint(GLFW_DECORATED, GL_TRUE);

#ifdef DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		apiWindow = apiCreateWindow(size, title.c_str(), nullptr);

		if (!apiWindow)logError("Window could not be created!");

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("Data/textures/ORKA Logo.png");

		if (glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else {
		logError("Window already exists!");
	}
}
void Window::create(String title, Area size, Bool decorated, WindowState state, ResourceManager& resourceManager)
{
	LifetimeGuard::create();

	this->decorated = decorated;
	this->windowState = state;
	id = nextWindowID++;
	windowedModeSize = size;
	createAPIWindow(title, size); //needs to be in this thread
	setCallbacks();
	thread.start(windowThread, resourceManager, *this);
	logEvent("Created Window in Main Thread!");
}

//key input
Bool Window::isKeyPressed(Int key) {
	return apiWindowKeyIsPressed(apiWindow, key);
}
Bool Window::pressed(InputID input) {
	switch (input.type) {
	case InputType::KeyBoard: return glfwGetKey(apiWindow, input.id); break;
	case InputType::Mouse: return mouseState[input.id]; break;
	default: logError("Unsupported InputType!");
	}
	return false;
}

//window getters
Bool Window::shouldClose()
{
	return glfwWindowShouldClose(apiWindow);
}
Bool Window::isCapturing() {
	return apiWindowIsCapturing(apiWindow);
}
Bool Window::isFullScreen()
{
	return apiWindowIsFullscreen(apiWindow);
}
Area Window::getFrameSize()
{
	return apiWindowGetWindowFrameSize(apiWindow);
}
Area Window::getContentSize()
{
	return apiWindowGetFramebufferSize(apiWindow);
}

//window callbacks
void whenWindowCloseRequest(APIWindow apiWindow) {
	glfwSetWindowShouldClose(apiWindow, true);
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

	if (!window.isFullScreen()) window.windowedModeSize = Area(width, height);

	if (apiWindowKeyIsPressed(apiWindow, LEFT_SHIFT)) {
		window.centerWindow();
	}
}
void whenMouseIsMoving(APIWindow apiWindow, Double mouseX, Double mouseY) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mousePos = MouseMovement(mouseX, mouseY);
	window.mousePosBotLeft = MouseMovement(mouseX, window.getContentSize().y - mouseY);
}
void whenMouseIsScrolling(APIWindow apiWindow, Double xAxis, Double yAxis) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	Int countX = abs(xAxis);
	Int countY = abs(yAxis);

	if (window.content) {

		for (Int i = 0; i < countX; i++) {
			window.content->inputEvent(window, InputEvent(InputType::Scroll, 0, xAxis > 0));
		}

		for (Int i = 0; i < countY; i++) {
			window.content->inputEvent(window, InputEvent(InputType::Scroll, 1, yAxis > 0));
		}
	}
}
void whenWindowContentScaleChanged(APIWindow apiWindow, Float xScale, Float yScale)
{
}
void whenFilesDroppedOnWindow(APIWindow apiWindow, Int count, const Char** droppedPaths)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	Vector<Path> paths;

	for (Int i = 0; i < count; i++) {
		paths.pushBack(droppedPaths[i]);
		logDebug(String("File dropped: ").append(paths.last().string()));
	}

	window.droppedFilePaths = paths;
}
void whenMouseIsPressed(APIWindow apiWindow, Int mouseButton, Int action, Int modifiers) {
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mouseState[mouseButton] = action > 0;
	if (window.content)window.content->inputEvent(window, InputEvent(InputType::Mouse, mouseButton, action));
}
void whenButtonIsPressed(APIWindow apiWindow, Int key, Int scancode, Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if (action == 2) return; //we do not process repeat button presses
	//[TODO] make sure that "typing events use them"

	InputEvent input = InputEvent(InputType::KeyBoard, key, action);

	if (input == window.escape) {
		window.keeprunning = false;
		apiSetWindowShouldClose(apiWindow, true);
	}
	if (input == window.enter && window.pressed(window.altKey)) {
		if (window.windowState == WindowState::windowed) window.windowState = WindowState::fullscreen;
		else window.windowState = WindowState::windowed;
		window.updateWindowState();
	}
	if (input == window.startProfiling) {
		window.profiling = true;
		OPTICK_START_CAPTURE();
	}
	if (input == window.stopProfiling) {
		window.profiling = false;
		OPTICK_STOP_CAPTURE();
		OPTICK_SAVE_CAPTURE("profileDump");
	}

	if (window.content)window.content->inputEvent(window, input);
}
