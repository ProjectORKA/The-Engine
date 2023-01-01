
#pragma once

#include "UIElement.hpp"
#include "UIButton.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UIImage : public UIElement {
	Name name;

	UIImage(Name name);
	void update(Window& window) override;
	void render(Engine& engine, Window& window, TiledRectangle renderArea)override;
};
struct UITextBox : public UIElement {
	String* data;

	UITextBox(String& data);
	void update(Window& window) override;
	void render(Engine& engine, Window& window, TiledRectangle renderArea) override;
};
struct UICheckBox : public UIElement {
	Boolean* data;
	UICheckBox(Boolean& data);
	void update(Window& window) override;
	void render(Engine& engine, Window& window, TiledRectangle renderArea) override;
};
struct UIContainer : public UIElement {
	Boolean renderVertical = false;

	Vector<UIElement*> contents;

	UIContainer& vertical();
	UIContainer& horizontal();
	UIContainer& insert(UIElement& element);
	void update(Window& window) override;
	void render(Engine& engine, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(Engine& engine, Window& window, TiledRectangle renderArea) override;
};

struct UserInterface {

	void run();

	UIElement* currentlyActive = nullptr;

	List<UIImage> images;
	List<Window> windows;
	List<UIButton> buttons;
	List<UITextBox> textBoxes;
	List<UICheckBox> checkBoxes;
	List<UIContainer> containers;

	UserInterface();
};

extern UserInterface ui;

UIButton& button();
UIContainer& container();
UIImage& image(Name name);
UICheckBox& checkBox(Bool& data);
UITextBox& textBox(String& data);
Window& window(String title, Area size, Bool decorated, WindowState state, Engine& engine);
Window& window(String title, Area size, Bool decorated, WindowState state, UIElement& element, Engine& engine);