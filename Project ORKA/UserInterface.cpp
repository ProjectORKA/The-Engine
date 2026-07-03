#include "UserInterface.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include <stdexcept>

UserInterface ui;

void UITextBox::create(Window& window) {}
void UITextBox::update(Window& window) {}
void UITextBox::destroy(Window& window) {}
void UITextBox::renderInteractive(Window& window) {}
void UITextBox::inputEvent(Window& window, InputEvent& input) {}

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

		// iterate over all windows while being able to delete them at the same time
		for (auto window = windows.begin(); window != windows.end();)
		{
			if (window->shouldClose())
			{
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

UITextBox::UITextBox(String& data)
{
	this->data = &data;
}

static Window* owningWindow(UIElement* element)
{
	for (UIElement* current = element; current != nullptr; current = current->parent)
	{
		if (auto* asWindow = dynamic_cast<Window*>(current)) return asWindow;
	}
	return nullptr;
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

void UITextBox::render(Window& window)
{
	Renderer& renderer = window.renderer;

	renderer.uiSpace(window);
	const IVec2 renderPos = getPosition() + IVec2(0, window.getHeight());
	const Matrix m = matrixFromPosition(Vec3(renderPos, 0));
	renderer.uniforms().setMMatrix(m);
	renderer.fontSize(16.0f);
	renderer.textRenderSystem.setLetterSpacing(0.6f);
	renderer.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	renderer.textRenderSystem.render(renderer, *data);
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
	apiWindowSetCursorPosition(focusedWindow->apiWindow, focusedWindow->mousePos);

	capturing = false;
	focusedElement = nullptr;
}

void UserInterface::capture(UIElement& element)
{
	Window* window = owningWindow(&element);
	if (!window) return;

	if (capturing) releaseCapture();
	focus(*window, &element);
	window->mousePos = apiWindowGetCursorPosition(window->apiWindow);
	apiWindowDisableCursor(window->apiWindow);
	capturing = true;
}
