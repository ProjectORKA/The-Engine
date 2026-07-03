#pragma once

#include "UIElement.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UITextBox final : UIElement
{
	String* data;

	explicit UITextBox(String& data);
	void     update(Window& window) override;
	void     create(Window& window) override;
	void     render(Window& window) override;
	void     destroy(Window& window) override;
	void     renderInteractive(Window& window) override;
	void     inputEvent(Window& window, InputEvent& input) override;
};

struct UserInterface
{
	List<Window> windows;
	Bool         running = false;
	Bool         glfwReady = false;
	Bool         capturing = false;
	Window*      focusedWindow = nullptr; //if capturing, this is the window that is capturing
	UIElement*   focusedElement = nullptr; //if capturing, this is the element that is capturing

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
};

extern UserInterface ui;
