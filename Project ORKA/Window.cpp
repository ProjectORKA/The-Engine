#include "Window.hpp"
#include "FileSystem.hpp"
#include "UserInterface.hpp"
#include "Profiler.hpp"

UShort nextWindowId = 0;

void Window::destroy()
{
	checkLifetime();
	keepRunning = false;
	thread.join();
	content.destroy(*this);
	destroyApiWindow();
	LifetimeGuard::destroy();
}

void Window::setWindowed()
{
	checkLifetime();

	apiWindowSetWindowedMode(apiWindow, TiledRectangle(windowedModeSize));
	if(isVisible) apiWindowRestore(apiWindow);
	centerWindow();
	windowState = WindowState::Windowed;
}

void Window::setMaximized()
{
	checkLifetime();

	apiMaximizeWindow(apiWindow);
	windowState = WindowState::Maximized;
}

void Window::centerWindow()
{
	checkLifetime();

	const TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - getFrameSize().center());
}

void Window::setMinimized()
{
	checkLifetime();

	apiMinimizeWindow(apiWindow);
	windowState = WindowState::Minimized;
}

void Window::captureCursor()
{
	checkLifetime();
	if(!capturing)
	{
		mousePos = apiWindowGetCursorPosition(apiWindow);
		apiWindowDisableCursor(apiWindow);
		capturing = true;
	}
}

void Window::setFullscreen()
{
	checkLifetime();

	glfwSetWindowMonitor(apiWindow, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	windowState = WindowState::Fullscreen;
}

void Window::decorateWindow()
{
	checkLifetime();

	apiWindowDecorate(apiWindow);
	decorated = true;
}

void Window::releaseCursor()
{
	if(capturing)
	{
		apiWindowEnableCursor(apiWindow);
		apiWindowSetCursorPosition(apiWindow, mousePos);
		capturing = false;
	}
}

void Window::unDecorateWindow()
{
	checkLifetime();
	apiWindowUndecorate(apiWindow);
	decorated = false;
}

void Window::destroyApiWindow()
{
	checkLifetime();

	releaseCursor();
	if(apiWindow)
	{
		glfwDestroyWindow(apiWindow);
		apiWindow = nullptr;
	}
	logDebug("API Window destroyed!");
}

void Window::updateWindowState()
{
	checkLifetime();

	switch(windowState)
	{
		case WindowState::Fullscreen:
			setFullscreen();
			break;
		case WindowState::Maximized:
			setMaximized();
			break;
		case WindowState::Windowed:
			setWindowed();
			break;
		case WindowState::Minimized:
			setMinimized();
			break;
	}
}

void Window::updateDecorations()
{
	checkLifetime();

	if(decorated) decorateWindow();
	else unDecorateWindow();
}

Bool Window::shouldClose() const
{
	return glfwWindowShouldClose(apiWindow);
}

Bool Window::isCapturing() const
{
	return apiWindowIsCapturing(apiWindow);
}

Bool Window::hasContent() const
{
	return !content.contents.empty();
}

void windowThread(Window& window)
{
	Renderer& renderer = window.renderer;
	window.initializeGraphicsApi(); // needs to be in this thread
	renderer.create(); // also needs to be in this thread
	window.updateWindowState();

	window.content.create(window);

	if(window.windowState == WindowState::Windowed) window.centerWindow();
	if(window.windowState == WindowState::Windowed || window.windowState == WindowState::Maximized) window.updateDecorations();

	auto prevPos = MouseMovement(0);

	OPTICK_THREAD("Window Thread");

	// main render loop for the window
	while(window.keepRunning)
	{
		OPTICK_FRAME("Window Frame");
		if(window.isVisible)
		{
			OPTICK_PUSH("Setup");

			window.mouseDelta = window.mousePosBotLeft - prevPos;
			prevPos           = window.mousePosBotLeft;

			const Area windowSize = window.getContentSize();

			const TiledRectangle windowArea(windowSize);

			renderer.begin(windowSize);

			OPTICK_POP();

			OPTICK_PUSH("Interactive Draw");
			window.content.update(window);
			OPTICK_POP();

			OPTICK_PUSH("Interactive Draw");
			window.content.renderInteractive(window, windowArea);
			OPTICK_POP();

			OPTICK_PUSH("Draw");
			window.content.render(window, windowArea);
			OPTICK_POP();

			if constexpr(USE_OPTICK)
			{
				renderer.aspectCorrectNormalizedSpace();
				renderer.textRenderSystem.setSize(0.02f);
				renderer.textRenderSystem.setLetterSpacing(0.6f);
				if(window.profiling) renderer.textRenderSystem.render(renderer, "[R]", Vec2(0.9f));
			}

			OPTICK_PUSH("Finalize");

			renderer.end(); // checks errors and unlocks renderer

			apiWindowSwapBuffers(window.apiWindow);
			OPTICK_POP();
		}
		window.mouseDelta = MouseMovement(0);
	}

	renderer.destroy();

	window.content.destroy(window);
}

void Window::setCallbacks() const
{
	checkLifetime();

	glfwSetKeyCallback(apiWindow, whenButtonIsPressed);
	glfwSetCursorPosCallback(apiWindow, whenMouseIsMoving);
	glfwSetScrollCallback(apiWindow, whenMouseIsScrolling);
	glfwSetDropCallback(apiWindow, whenFilesDroppedOnWindow);
	glfwSetMouseButtonCallback(apiWindow, whenMouseIsPressed);
	glfwSetWindowCloseCallback(apiWindow, whenWindowCloseRequest);
	glfwSetFramebufferSizeCallback(apiWindow, whenFramebufferIsResized);
}

Bool Window::isFullScreen() const
{
	return apiWindowIsFullscreen(apiWindow);
}

Area Window::getFrameSize() const
{
	return apiWindowGetWindowFrameSize(apiWindow);
}

void Window::updatePosition() const
{
	checkLifetime();

	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
}

Area Window::getContentSize() const
{
	return apiWindowGetFramebufferSize(apiWindow);
}

void Window::insert(UIElement& element)
{
	content.insert(element);
}

void Window::initializeGraphicsApi() const
{
	checkLifetime();

	glfwMakeContextCurrent(apiWindow);

	glfwSwapInterval(0);
}

void Window::setVisible(const Bool visible)
{
	checkLifetime();
	isVisible = visible;
	apiWindowSetVisibility(apiWindow, isVisible);
}

void Window::setIcon(const Path& path) const
{
	checkLifetime();

	Image logo;
	logo.loadOther(makeAbsolute(path), false); // [TODO] replace by "load()" if PNGs are loaded correctly

	if(logo.getDataType() == ImageDataType::Byte)
	{
		GLFWimage icon;
		icon.pixels = logo.getDataPointer();
		icon.width  = static_cast<Int>(logo.getWidth());
		icon.height = static_cast<Int>(logo.getHeight());
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else logWarning("Logo could not be Loaded!");
}

void Window::setPosition(const IVec2 position)
{
	checkLifetime();

	windowPosition = position;
	updatePosition();
}

Bool Window::isKeyPressed(const Int key) const
{
	return apiWindowKeyIsPressed(apiWindow, key);
}

Bool Window::pressed(const InputId input) const
{
	switch(input.type)
	{
		case InputType::KeyBoard:
			return glfwGetKey(apiWindow, input.id);
		case InputType::Mouse:
			return mouseState[input.id];
		default:
			logError("Unsupported InputType!");
	}
	return false;
}

void Window::setTitle(const String& string) const
{
	glfwSetWindowTitle(apiWindow, string.c_str());
}

void whenWindowCloseRequest(const APIWindow apiWindow)
{
	glfwSetWindowShouldClose(apiWindow, true);
}

void Window::resize(const Int width, const Int height) const
{
	checkLifetime();

	apiWindowResize(apiWindow, width, height);
}

void Window::createApiWindow(const String& title, const Area size)
{
	checkLifetime();

	if(!apiWindow)
	{
		// video mode
		const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		// opengl stuff
		glfwWindowHint(GLFW_SAMPLES, 0);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
		glfwWindowHint(GLFW_CENTER_CURSOR, true);
		glfwWindowHint(GLFW_VISIBLE, false);
		if(!decorated) glfwWindowHint(GLFW_DECORATED, false);
		else glfwWindowHint(GLFW_DECORATED, true);

		if(debugLoggingIsEnabled) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		apiWindow = apiCreateWindow(size, title.c_str(), nullptr);

		if(!apiWindow) logError("Window could not be created!");

		glfwSetWindowUserPointer(apiWindow, this);

		setIcon("Data/textures/ORKA Logo.png");

		if(glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else logError("Window already exists!");
}

void whenFramebufferIsResized(const APIWindow apiWindow, const Int width, const Int height)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if(!window.isFullScreen()) window.windowedModeSize = Area(width, height);

	if(apiWindowKeyIsPressed(apiWindow, LEFT_SHIFT)) window.centerWindow();

	window.renderer.sync();
}

void whenMouseIsMoving(const APIWindow apiWindow, const Double mouseX, const Double mouseY)
{
	Window& window         = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mousePos        = MouseMovement(mouseX, mouseY);
	window.mousePosBotLeft = MouseMovement(mouseX, window.getContentSize().y - mouseY);
}

void whenMouseIsScrolling(const APIWindow apiWindow, const Double xAxis, const Double yAxis)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	const Int countX = abs(static_cast<Int>(xAxis));
	const Int countY = abs(static_cast<Int>(yAxis));

	for(Int i = 0; i < countX; i++) window.content.inputEvent(window, InputEvent(InputType::Scroll, 0, xAxis > 0));
	for(Int i = 0; i < countY; i++) window.content.inputEvent(window, InputEvent(InputType::Scroll, 1, yAxis > 0));
}

void whenFilesDroppedOnWindow(const APIWindow apiWindow, const Int count, const Char** paths)
{
	Window&      window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	Vector<Path> pathVector;

	for(Int i = 0; i < count; i++)
	{
		pathVector.emplace_back(paths[i]);
		logDebug("File dropped: " + pathVector.back().string());
		pathVector.push_back(paths[i]);
		if(debugWindowIsEnabled) logDebug("File dropped: " + pathVector.back().string());
	}

	window.droppedFilePaths = pathVector;
}

void whenMouseIsPressed(const APIWindow apiWindow, const Int mouseButton, const Int action, Int modifiers)
{
	Window& window                 = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mouseState[mouseButton] = action > 0;
	window.content.inputEvent(window, InputEvent(InputType::Mouse, mouseButton, action));
}

void whenButtonIsPressed(const APIWindow apiWindow, const Int key, Int scanCode, const Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if(action == 2) return; // we do not process repeat button presses
	// [TODO] make sure that "typing events" use them

	const auto input = InputEvent(InputType::KeyBoard, key, action);

	if(input == window.escape)
	{
		window.keepRunning = false;
		apiSetWindowShouldClose(apiWindow, true);
	}
	if(input == window.enter && window.pressed(window.altKey))
	{
		if(window.windowState == WindowState::Windowed) window.windowState = WindowState::Fullscreen;
		else window.windowState                                            = WindowState::Windowed;
		window.updateWindowState();
	}
	if(input == window.startProfiling)
	{
		window.profiling = true;
		OPTICK_START_CAPTURE() }
	if(input == window.stopProfiling)
	{
		window.profiling = false;
		OPTICK_STOP_CAPTURE() OPTICK_SAVE_CAPTURE("profileDump");
	}
	if(input == window.reloadShaders) window.renderer.shaderSystem.rebuild();

	window.content.inputEvent(window, input);
}

void Window::create(const String& title, const Area size, const Bool decorated, const Bool visible, const WindowState state)
{
	LifetimeGuard::create();
	this->isVisible        = visible;
	this->decorated        = decorated;
	this->windowState      = state;
	this->id               = nextWindowId++;
	this->windowedModeSize = size;
	createApiWindow(title, size);	// needs to be in this thread
	setVisible(visible);			// glfw does weird stuff, so we need to force it
	setCallbacks();
	thread = Thread(windowThread, std::ref(*this));
	logDebug("Created Window in Main Thread!");
}
