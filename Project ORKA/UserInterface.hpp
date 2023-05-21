#pragma once

#include "UIElement.hpp"
#include "UIButton.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UIImage : UiElement
{
	Name name;

	UIImage(const Name& name);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UITextBox : UiElement
{
	String* data;

	UITextBox(String& data);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UICheckBox : UiElement
{
	Bool* data;
	UICheckBox(Bool& data);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UIContainer : UiElement
{
	Bool renderVertical = false;

	Vector<UiElement*> contents;

	UIContainer& vertical();
	UIContainer& horizontal();
	UIContainer& insert(UiElement& element);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UserInterface
{
	Bool initialized = false;
	UiElement* currentlyActive = nullptr;

	List<UIImage> images;
	List<Window> windows;
	List<UIButton> buttons;
	List<UITextBox> textBoxes;
	List<UICheckBox> checkBoxes;
	List<UIContainer> containers;

	void create()
	{
		if(glfwInit() != GLFW_TRUE)
		{
			logError("GLFW could not be initialized");
			return;
		}
		glfwSetErrorCallback(whenWindowAPIThrowsError);

		initialized = true;
	}

	void run();

	UIButton& button();
	UIContainer& container();
	UIImage& image(Name name);
	UICheckBox& checkBox(Bool& data);
	UITextBox& textBox(String& data);
	Window& window(const String& title, Area size, Bool decorated, WindowState state, ResourceManager& resourceManager);
	Window& window(const String& title, Area size, Bool decorated, WindowState state, UiElement& element, ResourceManager& resourceManager);
};