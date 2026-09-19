#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Agent/AgentBridge.hpp"
#include <stdexcept>

UserInterface ui;

Bool UserInterface::ensureInitialized()
{
	if (glfwReady) return true;
	if (External::glfwInit() != GLFW_TRUE)
	{
		logError("GLFW could not be initialized");
		return false;
	}
	glfwReady = true;
	logDebug("GLFW initialized!");
	External::glfwSetErrorCallback(whenWindowApiThrowsError);
	return true;
}

void UserInterface::terminate()
{
	if (!glfwReady) return;
	External::glfwTerminate();
	glfwReady = false;
}

void UserInterface::run(const Bool terminateOnExit)
{
	// as long as windows exist
	while (!windows.empty())
	{
		// collect input
		External::glfwPollEvents();
		agentBridge.drainMain();

		// iterate over all windows while being able to delete them at the same time
		for (auto window = windows.begin(); window != windows.end();)
		{
			if (window->shouldClose())
			{
				clearFocusForWindow(*window);
				window->destroy();
				window = windows.erase(window);
			}
			else ++window;
		}
	}

	for (Window& window : windows) window.destroy();
	windows.clear();
	External::glfwPollEvents();
	if (terminateOnExit) terminate();
}

UserInterface::UserInterface() { ensureInitialized(); }

static Window* owningWindow(UIElement* element)
{
	for (UIElement* current = element; current != nullptr; current = current->parent)
	{
		if (auto* asWindow = dynamic_cast<Window*>(current)) return asWindow;
	}
	return nullptr;
}

Bool uiElementBelongsToWindow(const UIElement* element, const Window& window)
{
	for (const UIElement* current = element; current != nullptr; current = current->parent)
	{
		if (current == &window) return true;
	}
	return false;
}

void UserInterface::clearFocusForWindow(const Window& window)
{
	if (focusedWindow == &window)
	{
		focusedWindow = nullptr;
		focusedElement = nullptr;
		return;
	}
	if (focusedElement && uiElementBelongsToWindow(focusedElement, window))
		focusedElement = nullptr;
}

static ApiWindow selectSharedContextWindow(const List<Window>& windows)
{
	if (!settings.shareOpenGLContexts) return nullptr;

	for (const Window& window : windows)
	{
		if (window.apiWindow != nullptr && window.apiWindowValid) return window.apiWindow;
	}
	return nullptr;
}

Window& UserInterface::window(const String& title, const Int width, const Int height, const Bool decorated, const Bool visible, const WindowState state)
{
	if (!ensureInitialized()) throw std::runtime_error("GLFW initialization failed");
	const ApiWindow sharedContextWindow = selectSharedContextWindow(windows);
	windows.emplace_back();
	Window& window = windows.back();
	if (!window.create(title, IVec2(width, height), decorated, visible, state, sharedContextWindow))
	{
		windows.pop_back();
		throw std::runtime_error("Window creation failed");
	}
	if (focusedWindow == nullptr) focusWindow(window);
	return window;
}

Window& UserInterface::window(const String& title, const Int width, const Int height, const Bool decorated, const Bool visible, const WindowState state, UIElement& content)
{
	if (!ensureInitialized()) throw std::runtime_error("GLFW initialization failed");
	const ApiWindow sharedContextWindow = selectSharedContextWindow(windows);
	windows.emplace_back();
	Window& window = windows.back();
	if (!window.create(title, IVec2(width, height), decorated, visible, state, sharedContextWindow))
	{
		windows.pop_back();
		throw std::runtime_error("Window creation failed");
	}
	window.addElement(content);
	if (focusedWindow == nullptr) focusWindow(window);
	return window;
}

void UserInterface::focus(Window& window, UIElement* element)
{
	focusedWindow         = &window;
	focusedElement        = element;
}

void UserInterface::focusWindow(Window& window)
{
	focusedWindow = &window;
}

void UserInterface::focusElement(Window& window, UIElement* element)
{
	focusedWindow         = &window;
	focusedElement        = element;
}

void UserInterface::releaseCapture()
{
	if (!capturing) return;

	if (!focusedElement || !focusedWindow) 
	{
		logError("There must be an element and window in focus!");
		return;
	}
	apiWindowEnableCursor(focusedWindow->apiWindow);
	apiWindowSetCursorPosition(
		focusedWindow->apiWindow, focusedWindow->mousePosition());

	capturing = false;
	focusedElement = nullptr;
}

void UserInterface::capture(UIElement& element)
{
	Window* window = owningWindow(&element);
	if (!window) return;

	if (capturing) releaseCapture();
	focus(*window, &element);
	window->publishMousePosition(
		apiWindowGetCursorPosition(window->apiWindow));
	apiWindowDisableCursor(window->apiWindow);
	capturing = true;
}
