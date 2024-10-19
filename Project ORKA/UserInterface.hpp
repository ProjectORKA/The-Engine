#pragma once

#include "UIElement.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UIImage final : UIElement
{
	Name name;

	UIImage(const Name& name);
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, TiledRectangle renderArea) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct UITextBox final : UIElement
{
	String* data;

	UITextBox(String& data);
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, TiledRectangle renderArea) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct UICheckBox : UIElement
{
	Bool* data;

	UICheckBox(Bool& data);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle renderArea) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct UserInterface
{
	UserInterface()
	{
		if (External::glfwInit() != GLFW_TRUE)
		{
			logError("GLFW could not be initialized");
			return;
		}
		logDebug("GLFW initialized!");
		External::glfwSetErrorCallback(whenWindowApiThrowsError);
	}

	void    run();
	Window& window(const String& title, Area size, Bool decorated, Bool visible, WindowState state);
	Window& window(const String& title, Area size, Bool decorated, Bool visible, WindowState state, UIElement& content);

	List<Window> windows;
	Bool         running = false;
};

extern UserInterface ui;
