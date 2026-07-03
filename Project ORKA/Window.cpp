#include "Window.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "OptickProfiling.hpp"
#include "UserInterface.hpp"
#include <iostream>

static UShort nextWindowId = 0;

static Vector<UIElement*> buildElementPath(UIElement* element)
{
	Vector<UIElement*> path;
	for (UIElement* current = element; current != nullptr; current = current->parent) path.push_back(current);
	return path;
}

static Bool pathContains(const Vector<UIElement*>& path, UIElement* element)
{
	for (UIElement* node : path)
	{
		if (node == element) return true;
	}
	return false;
}

static void updateHoverTarget(Window& window, UIElement* newTarget)
{
	UIElement* previous = window.hoveredElement;
	if (previous == newTarget) return;

	Vector<UIElement*> previousPath = buildElementPath(previous);
	Vector<UIElement*> newPath      = buildElementPath(newTarget);

	for (UIElement* element : previousPath)
	{
		if (!pathContains(newPath, element))
		{
			logDebug("Mouse left " + element->getDebugName());
			element->setHovered(false);
			element->onMouseLeave(window);
		}
	}

	for (auto it = newPath.rbegin(); it != newPath.rend(); ++it)
	{
		UIElement* element = *it;
		if (!pathContains(previousPath, element))
		{
			logDebug("Mouse entered " + element->getDebugName());
			element->setHovered(true);
			element->onMouseEnter(window);
		}
	}

	const String targetName = newTarget ? newTarget->getDebugName() : "null";
	logDebug("Mouse is now hovering over " + targetName);

	window.hoveredElement = newTarget;
}

void Window::destroy()
{
	keepRunning = false;
	if (thread.joinable()) thread.join();

	ui.releaseCapture();
	if (apiWindow)
	{
		glfwDestroyWindow(apiWindow);
		apiWindow = nullptr;
	}
	apiWindowValid = false;
	if (debugWindowIsEnabled) logDebug("API Window destroyed!");
}

void Window::setWindowed()
{
	apiWindowSetWindowedMode(apiWindow, TiledRectangle(storedWindowedModeSize));
	if (isVisible) apiWindowRestore(apiWindow);
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
	const TiledRectangle workableArea = apiWindowGetWorkArea(apiWindow);
	setPosition(workableArea.center() - TiledRectangle(getFrameSize()).center());
}

void Window::setMinimized()
{
	apiMinimizeWindow(apiWindow);
	windowState = WindowState::Minimized;
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

void Window::unDecorateWindow()
{
	apiWindowUndecorate(apiWindow);
	decorated = false;
}

void Window::updateWindowState()
{
	switch (windowState)
	{
	case WindowState::Fullscreen: setFullscreen();
		break;
	case WindowState::Maximized: setMaximized();
		break;
	case WindowState::Windowed: setWindowed();
		break;
	case WindowState::Minimized: setMinimized();
		break;
	}
}

void Window::updateDecorations()
{
	if (decorated) decorateWindow();
	else unDecorateWindow();
}

Bool Window::shouldClose() const
{
	if (!apiWindow) return true;
	return glfwWindowShouldClose(apiWindow);
}

Bool Window::isCapturing() const
{
	return apiWindowIsCapturing(apiWindow);
}

Bool Window::hasContent() const
{
	return !children.empty();
}

void windowThread(Window& window)
{
	Renderer& renderer = window.renderer;
	window.initializeGraphicsApi(); // needs to be in this thread
	renderer.create();              // also needs to be in this thread
	window.updateWindowState();

	window.UIElement::create(window);

	if (window.windowState == WindowState::Windowed) window.centerWindow();
	if (window.windowState == WindowState::Windowed || window.windowState == WindowState::Maximized) window.updateDecorations();

	auto prevPos = DVec2(0);

	OPTICK_THREAD("Window Thread");

	// main render loop for the window
	while (window.keepRunning)
	{
		OPTICK_FRAME("Window Frame");
		if (window.isVisible)
		{

			window.mouseDelta      = window.mousePosBotLeft - prevPos;
			prevPos                = window.mousePosBotLeft;
			const IVec2 windowSize = window.getContentSize();
			// When the window is minimized or being closed, the framebuffer size can
			// temporarily report zero. Skip rendering for that frame to avoid
			// downstream zero-sized render targets.
			if (windowSize.x <= 0 || windowSize.y <= 0)
			{
				window.mouseDelta = DVec2(0);
				continue;
			}
			window.setWidth(windowSize.x);
			window.setHeight(windowSize.y);

			renderer.begin(window.getSize());
			renderer.clearBackground(window.getBackgroundColor());
			renderer.setAlphaBlending(true);


			// UI calculations
			window.updateSizes();
			window.growSizes();
			window.updatePositions();

			window.update(window);
			window.renderInteractive(window);
			window.render(window);

			if constexpr (USE_OPTICK)
			{
				renderer.aspectCorrectNormalizedSpace();
				renderer.fontSize(0.02f);
				renderer.letterSpacing(0.6f);
				if (window.profiling) renderer.text("[R]", Vec2(0.9f));
			}

			OPTICK_PUSH("Finalize");

			renderer.end(window); // checks errors and unlocks renderer

			apiWindowSwapBuffers(window.apiWindow);
			OPTICK_POP();
		}
		window.mouseDelta = DVec2(0);
	}

	renderer.destroy();

	window.UIElement::destroy(window);
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

IVec2 Window::getFrameSize() const
{
	// framesize should not be used for content rendering since it contains the size of the border and top bar
	return apiWindowGetWindowFrameSize(apiWindow);
}

void Window::updatePosition() const
{
	glfwSetWindowPos(apiWindow, windowPosition.x, windowPosition.y);
}

IVec2 Window::getContentSize() const
{
	return apiWindowGetFramebufferSize(apiWindow);
}

void Window::initializeGraphicsApi() const
{
	glfwMakeContextCurrent(apiWindow);

	External::glfwSwapInterval(settings.vsyncEnabled ? 1 : 0);
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
	if (logo.getDataType() == ImageDataType::Byte)
	{
		External::GLFWimage icon;
		icon.pixels = logo.getDataPointer();
		icon.width  = static_cast<Int>(logo.getWidth());
		icon.height = static_cast<Int>(logo.getHeight());
		glfwSetWindowIcon(apiWindow, 1, &icon);
	}
	else logWarning("Logo could not be Loaded!");
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
	switch (input.type)
	{
	case InputType::KeyBoard: return glfwGetKey(apiWindow, input.id);
	case InputType::Mouse: return mouseState[input.id];
	default: logError("Unsupported InputType!");
	}
	return false;
}

UIElement* Window::pickElementAt(const DVec2& point)
{
	return findDeepestAt(*this, point);
}

Bool Window::routeInput(InputEvent& input)
{
	// Global capture: route all input directly to the capturing element chain.
	if (ui.capturing)
	{
		if (ui.focusedElement)
		{
			
		for (UIElement* element = ui.focusedElement; element != nullptr; element = element->parent)
		{
			element->inputEvent(*this, input);
			if (input.isConsumed()) return true;
		}
		return input.isConsumed();
		}
		logError("Capturing without focused element!");
		return false;
	}

	// mouse down updates focus
	if (input.id.type == InputType::Mouse && input.state)
	{
		UIElement* clicked = pickElementAt(mousePosBotLeft);
		ui.focus(*this, clicked ? clicked : this);
	}

	UIElement* target = nullptr;
	if (input.id.type == InputType::Mouse || input.id.type == InputType::Scroll) target = pickElementAt(mousePosBotLeft);
	if (!target && ui.focusedWindow == this && ui.focusedElement) target = ui.focusedElement;
	if (!target && ui.focusedElement) target = ui.focusedElement;
	if (!target && !children.empty()) target = children.back();
	if (!target) target = this;

	for (UIElement* element = target; element != nullptr; element = element->parent)
	{
		element->inputEvent(*this, input);
		if (input.isConsumed()) return true;
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

void Window::createApiWindow(const String& title, const IVec2 size, const ApiWindow sharedContextWindow)
{
	if (!apiWindow)
	{
		// video mode
		const External::GLFWvidmode* videoMode = glfwGetVideoMode(External::glfwGetPrimaryMonitor());
		External::glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		External::glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		External::glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		External::glfwWindowHint(GLFW_ALPHA_BITS, 8);
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
		External::glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, settings.transparentFramebufferEnabled ? GLFW_TRUE : GLFW_FALSE);
		External::glfwWindowHint(GLFW_CENTER_CURSOR, true);
		External::glfwWindowHint(GLFW_VISIBLE, false);
		if (!decorated) External::glfwWindowHint(GLFW_DECORATED, false);
		else External::glfwWindowHint(GLFW_DECORATED, true);

		if (debugLoggingIsEnabled) External::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		apiWindow = apiCreateWindow(size, title.c_str(), sharedContextWindow);

		if (!apiWindow) logError("Window could not be created!");

		glfwSetWindowUserPointer(apiWindow, this);

		const Path iconPath = resourceManager.getOrkaDataPath().string() + "\\textures\\ORKA Logo.png";

		setIcon(iconPath);

		if (External::glfwRawMouseMotionSupported()) glfwSetInputMode(apiWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else logError("Window already exists!");
}

void whenFramebufferIsResized(const ApiWindow apiWindow, const Int width, const Int height)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if (!window.isFullScreen()) window.storedWindowedModeSize = IVec2(width, height);

	if (apiWindowKeyIsPressed(apiWindow, LeftShift)) window.centerWindow();

	window.renderer.sync();
}

void whenMouseIsMoving(const ApiWindow apiWindow, const Double mouseX, const Double mouseY)
{
	Window& window         = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mousePos        = DVec2(mouseX, mouseY);
	window.mousePosBotLeft = DVec2(mouseX, window.getHeight() - mouseY);

	// While captured anywhere, ignore hover updates to avoid invisible cursor interactions.
	if (ui.capturing) return;

	UIElement* newHover = window.pickElementAt(window.mousePosBotLeft);
	updateHoverTarget(window, newHover);
}

void whenMouseIsScrolling(const ApiWindow apiWindow, const Double xAxis, const Double yAxis)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	const Int countX = abs(static_cast<Int>(xAxis));
	const Int countY = abs(static_cast<Int>(yAxis));

	for (Int i = 0; i < countX; i++)
	{
		InputEvent input(InputType::Scroll, 0, xAxis > 0);
		window.routeInput(input);
	}
	for (Int i = 0; i < countY; i++)
	{
		InputEvent input(InputType::Scroll, 1, yAxis > 0);
		window.routeInput(input);
	}
}

void whenFilesDroppedOnWindow(const ApiWindow apiWindow, const Int count, const Char** paths)
{
	Window&      window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	Vector<Path> pathVector;

	for (Int i = 0; i < count; i++)
	{
		pathVector.emplace_back(paths[i]);
		logDebug("File dropped: " + pathVector.back().string());
		if (debugWindowIsEnabled) logDebug("File dropped: " + pathVector.back().string());
	}

	window.droppedFilePaths = pathVector;
}

void whenMouseIsPressed(const ApiWindow apiWindow, const Int mouseButton, const Int action, Int modifiers)
{
	Window& window                 = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.mouseState[mouseButton] = action > 0;
	InputEvent input(InputType::Mouse, mouseButton, action);
	window.routeInput(input);
}

void whenButtonIsPressed(const ApiWindow apiWindow, const Int key, Int scanCode, const Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if (action == 2) return; // we do not process repeat button presses
	// [TODO] make sure that "typing events" use them

	auto input = InputEvent(InputType::KeyBoard, key, action);

	window.routeInput(input);
	if (input.isConsumed()) return;

	if (input == window.escape)
	{
		window.keepRunning = false;
		apiSetWindowShouldClose(apiWindow, true);
	}
	if (input == window.enter && window.pressed(window.altKey))
	{
		if (window.windowState == WindowState::Windowed) window.windowState = WindowState::Fullscreen;
		else window.windowState                                             = WindowState::Windowed;
		window.updateWindowState();
		// Switching monitor mode can reset cursor mode, so restore lock if this window owns capture.
		if (ui.capturing && ui.focusedWindow == &window) apiWindowDisableCursor(window.apiWindow);
	}
	if (input == window.startProfiling)
	{
		window.profiling = true;
		OPTICK_START_CAPTURE() }
	if (input == window.stopProfiling)
	{
		window.profiling = false;
		OPTICK_STOP_CAPTURE() OPTICK_SAVE_CAPTURE("profileDump");
	}
	if (input == window.reloadShaders) window.renderer.shaderSystem.rebuild(window.renderer);
	if (input == window.openGLLogging)
	{
		window.openGLLoggingEnabled = true;
		window.renderer.openGlContext.enableLogging();
		logDebug("OpenGL logging enabled - capturing all OpenGL calls for this frame");
	}
}

Bool Window::create(const String& title, const IVec2 size, const Bool decorated, const Bool visible, const WindowState state, const ApiWindow sharedContextWindow)
{
	this->storedWindowedModeSize = size;
	this->windowState            = state;
	this->isVisible              = visible;
	this->decorated              = decorated;
	this->id                     = nextWindowId++;
	createApiWindow(title, size, sharedContextWindow); // needs to be in this thread
	apiWindowValid = apiWindow != nullptr;

	if (!apiWindowValid)
	{
		keepRunning = false;
		return false;
	}

	setVisible(visible); // glfw does weird stuff, so we need to force it
	setCallbacks();
	thread = Thread(windowThread, std::ref(*this));
	logDebug("Created Window in Main Thread!");
	return true;
}

Color Window::getBackgroundColor() const
{
	return backgroundColor;
}
