#pragma once

#include "UIElement.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UserInterface
{
	List<Window> windows;
	Bool         running = false;
	Bool         glfwReady = false;
	Atomic<Bool> capturing = false;
	Window*      focusedWindow = nullptr; //if capturing, this is the window that is capturing
	UIElement*   focusedElement = nullptr; //if capturing, this is the element that is capturing
	DVec2        capturedCursorPosition = DVec2(0); // logical position before cursor lock

	UserInterface();

	void    run(Bool terminateOnExit = true);
	Bool    ensureInitialized();
	void    terminate();
	Window& window(const String& title, Int width, Int height, Bool decorated, Bool visible, WindowState state);
	Window& window(const String& title, Int width, Int height, Bool decorated, Bool visible, WindowState state, UIElement& content);

	void focus(Window& window, UIElement* element);
	void focusWindow(Window& window);
	void focusElement(Window& window, UIElement* element);
	void capture(UIElement& element);
	void releaseCapture();
	void clearFocusForWindow(const Window& window);
};

[[nodiscard]] Bool uiElementBelongsToWindow(const UIElement* element, const Window& window);

extern UserInterface ui;
