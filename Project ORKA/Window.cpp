#include "Window.hpp"
#include "Renderer.hpp"
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "OptickProfiling.hpp"
#include "UserInterface.hpp"
#include "UI/UiHost.hpp"
#include "Debug.hpp"
#include "Agent/AgentBridge.hpp"
#include <bit>
#include <chrono>
#include <iostream>
#include <thread>

static UShort nextWindowId = 0;

Window::Window()
	: rendererOwner_(std::make_unique<Renderer>())
	, renderer(*rendererOwner_)
{
}

Window::~Window() = default;

namespace
{
	static_assert(Atomic<U64>::is_always_lock_free,
		"Pointer publication must remain lock-free");

	U64 packMousePosition(const DVec2 position)
	{
		const U64 x = std::bit_cast<U32>(static_cast<Float>(position.x));
		const U64 y = std::bit_cast<U32>(static_cast<Float>(position.y));
		return (x << 32) | y;
	}

	DVec2 unpackMousePosition(const U64 packed)
	{
		return DVec2(
			std::bit_cast<Float>(static_cast<U32>(packed >> 32)),
			std::bit_cast<Float>(static_cast<U32>(packed)));
	}
}

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
			if (debugMouseHoverIsEnabled) logDebug("Mouse left " + element->getDebugName());
			element->setHovered(false);
			element->onMouseLeave(window);
		}
	}

	for (auto it = newPath.rbegin(); it != newPath.rend(); ++it)
	{
		UIElement* element = *it;
		if (!pathContains(previousPath, element))
		{
			if (debugMouseHoverIsEnabled) logDebug("Mouse entered " + element->getDebugName());
			element->setHovered(true);
			element->onMouseEnter(window);
		}
	}

	const String targetName = newTarget ? newTarget->getDebugName() : "null";
	if (debugMouseHoverIsEnabled) logDebug("Mouse is now hovering over " + targetName);

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

	OPTICK_THREAD("Window Thread");

	// main render loop for the window
	while (window.keepRunning)
	{
		OPTICK_FRAME("Window Frame");
		if (window.isVisible)
		{
			const auto agentFrameStart = std::chrono::steady_clock::now();
			window.agentFrameIndex.fetch_add(1);

			window.sampleMousePositionForFrame();
			const IVec2 windowSize = window.getContentSize();
			// When the window is minimized or being closed, the framebuffer size can
			// temporarily report zero. Skip rendering for that frame to avoid
			// downstream zero-sized render targets.
			if (windowSize.x <= 0 || windowSize.y <= 0)
			{
				window.mouseDelta = DVec2(0);
				// Avoid spinning at 100% CPU while minimized or during close teardown.
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				continue;
			}
			window.setWidth(windowSize.x);
			window.setHeight(windowSize.y);

			renderer.begin(window.getSize(), window.getBackgroundColor());
			renderer.setAlphaBlending(true);


			// UI calculations
			const auto layoutStart = std::chrono::steady_clock::now();
			window.updateSizes();
			window.growSizes();
			window.updatePositions();
			const auto updateStart = std::chrono::steady_clock::now();

			window.update(window);
			const auto renderStart = std::chrono::steady_clock::now();
			window.renderInteractive(window);
			window.render(window);
			const auto finalizeStart = std::chrono::steady_clock::now();

			if constexpr (USE_OPTICK)
			{
				renderer.aspectCorrectNormalizedSpace();
				renderer.fontSize(0.02f);
				renderer.letterSpacing(0.6f);
				if (window.profiling) renderer.text("[R]", Vec2(0.9f));
			}

			OPTICK_PUSH("Finalize");

			renderer.end(window); // checks errors and unlocks renderer
			agentBridge.drainRender(window);

			apiWindowSwapBuffers(window.apiWindow);
			const auto frameEnd = std::chrono::steady_clock::now();
			const auto milliseconds = [](const auto begin, const auto end)
			{
				return std::chrono::duration<Double, std::milli>(end - begin).count();
			};
			agentBridge.recordFrame({
				window.id,
				window.agentFrameIndex.load(),
				milliseconds(agentFrameStart, frameEnd),
				milliseconds(layoutStart, updateStart),
				milliseconds(updateStart, renderStart),
				milliseconds(renderStart, finalizeStart),
				milliseconds(finalizeStart, frameEnd)
			});
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
	glfwSetCharCallback(apiWindow, whenUnicodeCharacterIsInput);
	glfwSetWindowFocusCallback(apiWindow, whenWindowFocusChanges);
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
	const Bool injected = key >= 0 && key < static_cast<Int>(injectedKeyState.size())
		&& injectedKeyState[key].load();
	return injected || apiWindowKeyIsPressed(apiWindow, key);
}

Bool Window::pressed(const InputId input) const
{
	switch (input.type)
	{
	case InputType::KeyBoard: return isKeyPressed(input.id);
	case InputType::Mouse:
		if (input.id < 0 || input.id >= static_cast<Int>(mouseButtonDown.size())) return false;
		return mouseButtonDown[input.id].load(std::memory_order_acquire);
	default: logError("Unsupported InputType!");
	}
	return false;
}

UIElement* Window::pickElementAt(const DVec2& point)
{
	return findDeepestAt(*this, point);
}

void Window::publishMousePosition(const DVec2 logicalPosition)
{
	publishedMousePosition.store(packMousePosition(logicalPosition), std::memory_order_release);
}

void Window::alignFrameMouseSample(const DVec2 logicalPosition)
{
	publishMousePosition(logicalPosition);
	frameMousePosition = logicalPosition;
	frameMousePositionBotLeft =
		DVec2(logicalPosition.x, getHeight() - static_cast<Float>(logicalPosition.y));
	mouseDelta = DVec2(0);
}

void Window::requestUiStandardCursor(const ApiCursorType cursor)
{
	uiStandardCursor.store(static_cast<Int>(cursor), std::memory_order_release);
}

void Window::requestUiCursorCapture(const Bool capture)
{
	uiCursorCaptureRequested.store(capture, std::memory_order_release);
}

void Window::applyUiCursorRequests()
{
	if (!apiWindow) return;

	const Bool gameCapture = ui.capturing && ui.focusedWindow == this;
	const Bool uiCapture = uiCursorCaptureRequested.load(std::memory_order_acquire);
	const Bool cursorDisabled = apiWindowCursorIsCaptured(apiWindow);
	const Bool cursorHidden = apiWindowCursorIsHidden(apiWindow);

	if (gameCapture)
	{
		if (!cursorDisabled) apiWindowDisableCursor(apiWindow);
		return;
	}

	// Stuck relative capture from a game while retained UI is idle.
	if (cursorDisabled)
	{
		const DVec2 restore = mousePosition();
		apiWindowEnableCursor(apiWindow);
		apiWindowSetCursorPosition(apiWindow, restore);
		alignFrameMouseSample(restore);
		if (debugUiCursorIsEnabled)
			logWarning("UiCursor: restored visible cursor (unexpected GLFW disabled mode)");
	}

	if (uiCapture)
	{
		if (!cursorHidden) apiWindowHideCursor(apiWindow);
	}
	else if (cursorHidden)
	{
		apiWindowEnableCursor(apiWindow);
	}

	if (!apiWindowCursorIsCaptured(apiWindow))
	{
		const ApiCursorType cursor = static_cast<ApiCursorType>(
			uiStandardCursor.load(std::memory_order_acquire));
		apiWindowSetStandardCursor(apiWindow, cursor);
	}
}

ApiCursorType Window::queuedStandardCursor() const
{
	return static_cast<ApiCursorType>(uiStandardCursor.load(std::memory_order_acquire));
}

Bool Window::uiCursorCapturePending() const
{
	return uiCursorCaptureRequested.load(std::memory_order_acquire);
}

Bool Window::systemCursorHiddenByGlfw() const
{
	return apiWindow ? apiWindowCursorIsCaptured(apiWindow) : false;
}

DVec2 Window::mousePosition() const
{
	return unpackMousePosition(publishedMousePosition.load(std::memory_order_acquire));
}

DVec2 Window::mousePositionBotLeft() const
{
	const DVec2 position = mousePosition();
	return DVec2(position.x, getHeight() - position.y);
}

void Window::sampleMousePositionForFrame()
{
	const DVec2 previous = frameMousePositionBotLeft;
	frameMousePosition = mousePosition();
	frameMousePositionBotLeft =
		DVec2(frameMousePosition.x, getHeight() - frameMousePosition.y);
	mouseDelta = frameMousePositionBotLeft - previous;

	// Layout, legacy hover state, retained hover state, camera deltas, and
	// rendering now all consume the same frame-local pointer sample.
	if (!ui.capturing)
		updateHoverTarget(*this, pickElementAt(frameMousePositionBotLeft));
}

Bool Window::routeInput(InputEvent& input)
{
	// Global capture stays on the legacy element chain (3D camera look).
	if (ui.capturing)
	{
		if (ui.focusedElement && uiElementBelongsToWindow(ui.focusedElement, *this))
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

	const Bool pointerEvent = input.id.type == InputType::Mouse || input.id.type == InputType::Scroll;
	UIElement* target = nullptr;
	if (pointerEvent) target = pickElementAt(mousePositionBotLeft());
	else if (ui.focusedElement && uiElementBelongsToWindow(ui.focusedElement, *this))
		target = ui.focusedElement;
	if (!target && !children.empty()) target = children.back();
	if (!target) target = this;

	Ui::UiHost* retainedHost = nullptr;
	for (UIElement* element = target; element != nullptr; element = element->parent)
	{
		if (auto* host = dynamic_cast<Ui::UiHost*>(element))
		{
			retainedHost = host;
			break;
		}
	}

	// Pointer hits on a retained host stay on that host. Keys are offered to the
	// host first; anything the document does not handle continues to legacy elements.
	if (retainedHost && (pointerEvent || input.id.type == InputType::KeyBoard ||
		input.id.type == InputType::Controller))
	{
		retainedHost->inputEvent(*this, input);
		if (input.isConsumed())
		{
			if (input.id.type == InputType::Mouse && input.state)
				ui.focus(*this, retainedHost);
			return true;
		}
	}

	if (input.id.type == InputType::Mouse && input.state)
	{
		UIElement* clicked = pickElementAt(mousePositionBotLeft());
		ui.focus(*this, clicked ? clicked : this);
	}

	for (UIElement* element = target; element != nullptr; element = element->parent)
	{
		if (element == retainedHost) continue;
		element->inputEvent(*this, input);
		if (input.isConsumed()) return true;
	}

	return false;
}

void Window::injectPointerMove(const DVec2 logicalPosition)
{
	apiWindowSetCursorPosition(apiWindow, logicalPosition);
	whenMouseIsMoving(apiWindow, logicalPosition.x, logicalPosition.y);
}

void Window::injectMouseButton(const Int button, const Bool down)
{
	if(button < 0 || button >= 8) throw std::runtime_error("Mouse button must be between 0 and 7");
	whenMouseIsPressed(apiWindow, button, down ? GLFW_PRESS : GLFW_RELEASE, 0);
}

void Window::injectScroll(const Double x, const Double y)
{
	whenMouseIsScrolling(apiWindow, x, y);
}

void Window::injectKey(const Int key, const Bool down, const Int modifiers, const Bool repeat)
{
	if(key < 0 || key >= static_cast<Int>(injectedKeyState.size()))
		throw std::runtime_error("Key code is outside the supported range");
	injectedKeyState[key].store(down);
	whenButtonIsPressed(apiWindow, key, 0,
		repeat ? GLFW_REPEAT : (down ? GLFW_PRESS : GLFW_RELEASE), modifiers);
}

void Window::injectText(const String& utf8) const
{
	for(SizeT i = 0; i < utf8.size();)
	{
		const Byte first = static_cast<Byte>(utf8[i++]);
		U32 codepoint = first;
		SizeT continuation = 0;
		if((first & 0xe0) == 0xc0) { codepoint = first & 0x1f; continuation = 1; }
		else if((first & 0xf0) == 0xe0) { codepoint = first & 0x0f; continuation = 2; }
		else if((first & 0xf8) == 0xf0) { codepoint = first & 0x07; continuation = 3; }
		else if(first > 0x7f) throw std::runtime_error("Text is not valid UTF-8");
		if(i + continuation > utf8.size()) throw std::runtime_error("Text is not valid UTF-8");
		for(SizeT j = 0; j < continuation; ++j)
		{
			const Byte next = static_cast<Byte>(utf8[i++]);
			if((next & 0xc0) != 0x80) throw std::runtime_error("Text is not valid UTF-8");
			codepoint = (codepoint << 6) | (next & 0x3f);
		}
		if(codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff))
			throw std::runtime_error("Text is not valid UTF-8");
		if(unicodeInputCallback) unicodeInputCallback(codepoint);
	}
}

void Window::forwardCompositionStart() const
{
	if (compositionStartCallback) compositionStartCallback();
}

void Window::forwardCompositionUpdate(const String& utf8, const SizeT selectionStart,
	const SizeT selectionEnd) const
{
	if (compositionUpdateCallback) compositionUpdateCallback(utf8, selectionStart, selectionEnd);
}

void Window::forwardCompositionCommit(const String& utf8) const
{
	if (compositionCommitCallback) compositionCommitCallback(utf8);
}

void Window::forwardCompositionCancel() const
{
	if (compositionCancelCallback) compositionCancelCallback();
}

void Window::setTitle(const String& string)
{
	title = string;
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
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	window.publishMousePosition(DVec2(mouseX, mouseY));
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
	if (mouseButton >= 0 && mouseButton < static_cast<Int>(window.mouseButtonDown.size()))
		window.mouseButtonDown[mouseButton].store(action > 0, std::memory_order_release);
	InputEvent input(InputType::Mouse, mouseButton, action);
	window.routeInput(input);
}

void whenButtonIsPressed(const ApiWindow apiWindow, const Int key, Int scanCode, const Int action, Int modifiers)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));

	if (window.keyInputCallback) window.keyInputCallback(key, scanCode, action, modifiers);
	if (action == GLFW_REPEAT) return; // legacy controls still intentionally ignore repeats

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

void whenUnicodeCharacterIsInput(const ApiWindow apiWindow, const UInt codepoint)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	if (window.unicodeInputCallback) window.unicodeInputCallback(codepoint);
}

void whenWindowFocusChanges(const ApiWindow apiWindow, const Int focused)
{
	Window& window = *static_cast<Window*>(glfwGetWindowUserPointer(apiWindow));
	if (window.focusInputCallback) window.focusInputCallback(focused == GLFW_TRUE);
}

Bool Window::create(const String& title, const IVec2 size, const Bool decorated, const Bool visible, const WindowState state, const ApiWindow sharedContextWindow)
{
	this->title                  = title;
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
	if (debugWindowIsEnabled) logDebug("Created Window in Main Thread!");
	return true;
}

Color Window::getBackgroundColor() const
{
	return backgroundColor;
}
