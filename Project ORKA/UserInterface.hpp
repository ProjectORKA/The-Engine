#pragma once

#include "UIElement.hpp"
#include "UIButton.hpp"
#include "Window.hpp"
#include "LifeTimeGuard.hpp"

struct Renderer;
struct Window;

struct UIImage : public UIElement {
	Name name;

	UIImage(const Name& name);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UITextBox : public UIElement {
	String* data;

	UITextBox(String& data);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UICheckBox : public UIElement {
	Boolean* data;
	UICheckBox(Boolean& data);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UIContainer : public UIElement {
	Boolean renderVertical = false;

	Vector<UIElement*> contents;

	UIContainer& vertical();
	UIContainer& horizontal();
	UIContainer& insert(UIElement& element);
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UserInterface {
	Bool initialized = false;
	UIElement* currentlyActive = nullptr;

	List<UIImage> images;
	List<Window> windows;
	List<UIButton> buttons;
	List<UITextBox> textBoxes;
	List<UICheckBox> checkBoxes;
	List<UIContainer> containers;

	void create() {
		if (glfwInit() != GLFW_TRUE) {
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
	Window& window(const String& title, Area size, Bool decorated, WindowState state, UIElement& element,
	               ResourceManager& resourceManager);
};
