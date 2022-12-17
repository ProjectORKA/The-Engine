
#include "Window.hpp"
#include "UserInterface.hpp"

#include "Profiler.hpp"

UShort nextWindowID = 0;

void finish() {
	OPTICK_EVENT();
	glFinish();
}

//window thread
void windowThread(Window& window)
{
	Renderer& renderer = window.renderer;
	window.initializeGraphicsAPI();					//needs to be in this thread
	renderer.create(window.getContentSize());		//also needs to be in this thread
	window.updateWindowState();

	if (window.content)window.content->create(window);

	if (window.windowState == WindowState::windowed) window.centerWindow();
	if (window.windowState == WindowState::windowed || window.windowState == WindowState::maximized) window.updateDecorations();

	window.setCallbacks();

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
			TiledRectangle windowArea;
			windowArea.position = IVec2(0);
			windowArea.size = window.getContentSize();
			renderer.framebufferSystem.update(windowArea.size);
			renderer.renderRegion.set(windowArea);

			//gpu needs frameSize
			renderer.uniforms().width(renderer.framebufferSystem.currentDraw().size.x);
			renderer.uniforms().height(renderer.framebufferSystem.currentDraw().size.y);

			OPTICK_POP();

			if (window.content) {
				//update ui

				window.content->update(window);

				//render interactibles
				OPTICK_PUSH("Interactive Draw");
				renderer.idFramebuffer.draw(renderer);
				renderer.useShader("idShader");
				renderer.uniforms().reset();
				renderer.renderMesh("fullScreenQuad");
				renderer.clearDepth();
				renderer.setWireframeMode(false);
				renderer.setCulling(false);
				renderer.useShader("idShader");
				renderer.setAlphaBlending(false);
				window.content->renderInteractive(window, windowArea);
				renderer.idFramebuffer.updateIdsUnderCursor(window);
				OPTICK_POP();

				//render ui
				OPTICK_PUSH("Draw");
				renderer.draw("main");
				renderer.clearColor(Color(Vec3(0), 0.0));
				renderer.clearDepth();
				renderer.setWireframeMode(renderer.wireframeMode);
				window.content->render(window, windowArea);

				renderer.aspectCorrectNormalizedSpace();
				if (window.profiling) renderer.renderText("[R]", Vec2(0.9), FontStyle(0.02));
				OPTICK_POP();
			}

			OPTICK_PUSH("Finalize");

			renderer.setWireframeMode(false);
			renderer.setAlphaBlending(false);

			renderer.renderRegion.set(windowArea);

			//renderer.framebufferSystem.current().blitFramebuffer();

			renderer.framebufferSystem.drawToWindow();


			renderer.useShader("final");
			renderer.framebufferSystem.currentDraw().setAsTexture(0); //[TODO] might not work; check
			renderer.uniforms().reset();
			renderer.renderMesh("fullScreenQuad");

			renderer.end(); //checks errors and unlocks renderer

			finish();

			apiWindowSwapBuffers(window.apiWindow);
			OPTICK_POP();
		}
		window.mouseDelta = MouseMovement(0);
	}
	renderer.destroy();
	window.destroyAPIWindow();
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
}
void Window::setWindowed()
{
	apiWindowSetWindowedMode(apiWindow, windowedModeSize);
	apiWindowRestore(apiWindow);
	centerWindow();
	windowState = WindowState::windowed;
}
void Window::setMaximized() {
	apiMaximizeWindow(apiWindow);
	windowState = WindowState::maximized;
}
void Window::setCallbacks()
{
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
	TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - getFrameSize().center());
}
void Window::setMinimized() {
	apiMinimizeWindow(apiWindow);
	windowState = WindowState::minimized;
}
void Window::setFullscreen() {
	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	windowState = WindowState::fullscreen;
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
	uncaptureCursor();
	if (apiWindow) {
		glfwDestroyWindow(apiWindow);
		apiWindow = nullptr;
	}
	logEvent("API Window destroyed!");
}
void Window::setIcon(Path path) {

	path = std::filesystem::absolute(path);

	Image logo = fileSystem.loadImage(path, 8, false);

	if (logo.pixels && logo.channels == 4) {
		GLFWimage icon;
		icon.pixels = logo.pixels;
		icon.width = logo.width;
		icon.height = logo.height;
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else {
		logWarning("Logo could not be Loaded!");
	}
}
void Window::updateWindowState() {
	switch (windowState) {
	case WindowState::fullscreen: setFullscreen(); break;
	case WindowState::maximized: setMaximized(); break;
	case WindowState::windowed: setWindowed(); break;
	case WindowState::minimized: setMinimized(); break;
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
Window& Window::insert(UIElement& element) {
	content = &element;
	return *this;
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
void Window::create(String title, Area size, Bool decorated, WindowState state)
{
	this->decorated = decorated;
	this->windowState = state;
	id = nextWindowID++;
	windowedModeSize = size;
	createAPIWindow(title, size); //needs to be in this thread

	thread.start(windowThread, *this);
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
		paths.push_back(droppedPaths[i]);
		logDebug(String("File dropped: ").append(paths.back().string()));
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

	if(window.content)window.content->inputEvent(window, input);
}
