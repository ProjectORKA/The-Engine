#include "Window.hpp"
#include "FileSystem.hpp"
#include "UserInterface.hpp"
#include "Profiler.hpp"
#include "ResourceManager.hpp"

UShort nextWindowId = 0;

void Window::destroy()
{
	keepRunning = false;
	thread.join();
	content.destroy(*this);
	destroyApiWindow();
	LifetimeGuard::destroy();
}

void Window::setWindowed()
{
	apiWindowSetWindowedMode(apiWindow, TiledRectangle(windowedModeSize));
	if(isVisible) apiWindowRestore(apiWindow);
	centerWindow();
	windowState = WindowState::Windowed;
}

void Window::setMaximized()
{
	apiMaximizeWindow(apiWindow);
	windowState = WindowState::Maximized;
}

void Window::centerWindow()
{
	const TiledRectangle workableArea = apiWindowGetWorkableArea(apiWindow);
	setPosition(workableArea.center() - getFrameSize().center());
}

void Window::setMinimized()
{
	apiMinimizeWindow(apiWindow);
	windowState = WindowState::Minimized;
}

void Window::captureCursor()
{
	if(!capturing)
	{
		mousePos = apiWindowGetCursorPosition(apiWindow);
		apiWindowDisableCursor(apiWindow);
		capturing = true;
	}
}

void Window::setFullscreen()
{
	glfwSetWindowMonitor(apiWindow, External::glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(External::glfwGetPrimaryMonitor())->width, External::glfwGetVideoMode(External::glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE);
	windowState = WindowState::Fullscreen;
}

void Window::decorateWindow()
{
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
	apiWindowUndecorate(apiWindow);
	decorated = false;
}

void Window::destroyApiWindow()
{
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
	if(decorated)
	{
		decorateWindow();
	}
	else
	{
		unDecorateWindow();
	}
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
	return !content.isEmpty();
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

	auto prevPos = DVec2(0);

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
		window.mouseDelta = DVec2(0);
	}

	renderer.destroy();

	window.content.destroy(window);
}

void Window::setCallbacks() const
{
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
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
}

Area Window::getContentSize() const
{
	return apiWindowGetFramebufferSize(apiWindow);
}

void Window::add(UIElement& element)
{
	content.add(element);
}

void Window::initializeGraphicsApi() const
{
	glfwMakeContextCurrent(apiWindow);

	External::glfwSwapInterval(0);
}

void Window::setVisible(const Bool visible)
{
	isVisible = visible;
	apiWindowSetVisibility(apiWindow, isVisible);
}

void Window::setIcon(const Path& path) const
{
	Image logo;
	logo.load(path);
	logo.flipVertically();
	if(logo.getDataType() == ImageDataType::Byte)
	{
		External::GLFWimage icon;
		icon.pixels = logo.getDataPointer();
		icon.width  = static_cast<Int>(logo.getWidth());
		icon.height = static_cast<Int>(logo.getHeight());
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else
	{
		logWarning("Logo could not be Loaded!");
	}
}

void Window::setPosition(const IVec2 position)
{
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

void whenWindowCloseRequest(const ApiWindow apiWindow)
{
	glfwSetWindowShouldClose(apiWindow, true);
}

void Window::resize(const Int width, const Int height) const
{
	apiWindowResize(apiWindow, width, height);
}

void Window::createApiWindow(const String& title, const Area size)
{
	if(!apiWindow)
	{
		// video mode
		const External::GLFWvidmode* videoMode = glfwGetVideoMode(External::glfwGetPrimaryMonitor());
		External::glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		External::glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		External::glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		External::glfwWindowHint(GLFW_DEPTH_BITS, 24);
		External::glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		// opengl stuff
		External::glfwWindowHint(GLFW_SAMPLES, 0);
		External::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		External::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		External::glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		External::glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
		External::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

		External::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		External::glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
		External::glfwWindowHint(GLFW_CENTER_CURSOR, true);
		External::glfwWindowHint(GLFW_VISIBLE, false);
		if(!decorated)
		{
			External::glfwWindowHint(GLFW_DECORATED, false);
		}
		else
		{
			External::glfwWindowHint(GLFW_DECORATED, true);
		}

		if(debugLoggingIsEnabled) External::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		apiWindow = apiCreateWindow(size, title.c_str(), nullptr);

		if(!apiWindow) logError("Window could not be created!");

		glfwSetWindowUserPointer(apiWindow, this);

		const Path iconPath = resourceManager.getOrkaDataPath().string() + "\\textures\\ORKA Logo.png";

		setIcon(iconPath);

		if(External::glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else
	{
		logError("Window already exists!");
	}
}

void whenFramebufferIsResized(const ApiWindow apiWindow, const Int width, const Int height)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if(!window.isFullScreen()) window.windowedModeSize = Area(width, height);

	if(apiWindowKeyIsPressed(apiWindow, LeftShift)) window.centerWindow();

	window.renderer.sync();
}

void whenMouseIsMoving(const ApiWindow apiWindow, const Double mouseX, const Double mouseY)
{
	Window& window         = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mousePos        = DVec2(mouseX, mouseY);
	window.mousePosBotLeft = DVec2(mouseX, window.getContentSize().y - mouseY);
}

void whenMouseIsScrolling(const ApiWindow apiWindow, const Double xAxis, const Double yAxis)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	const Int countX = abs(static_cast<Int>(xAxis));
	const Int countY = abs(static_cast<Int>(yAxis));

	for(Int i = 0; i < countX; i++) window.content.inputEvent(window, InputEvent(InputType::Scroll, 0, xAxis > 0));
	for(Int i = 0; i < countY; i++) window.content.inputEvent(window, InputEvent(InputType::Scroll, 1, yAxis > 0));
}

void whenFilesDroppedOnWindow(const ApiWindow apiWindow, const Int count, const Char** paths)
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

void whenMouseIsPressed(const ApiWindow apiWindow, const Int mouseButton, const Int action, Int modifiers)
{
	Window& window                 = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mouseState[mouseButton] = action > 0;
	window.content.inputEvent(window, InputEvent(InputType::Mouse, mouseButton, action));
}

void whenButtonIsPressed(const ApiWindow apiWindow, const Int key, Int scanCode, const Int action, Int modifiers)
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
		if(window.windowState == WindowState::Windowed)
		{
			window.windowState = WindowState::Fullscreen;
		}
		else
		{
			window.windowState                                            = WindowState::Windowed;
		}
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
