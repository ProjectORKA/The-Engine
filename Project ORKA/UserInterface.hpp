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
	void     inputEvent(Window& window, InputEvent input) override;
};

struct UserInterface
{
	List<Window> windows;
	Bool         running = false;

	UserInterface();

	void    run();
	Window& window(const String& title, Int width, Int height, Bool decorated, Bool visible, WindowState state);
	Window& window(const String& title, Int width, Int height, Bool decorated, Bool visible, WindowState state, UIElement& content);
};

extern UserInterface ui;
