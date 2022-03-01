
#pragma once

#include "Basics.hpp"
#include "GPUTexture.hpp"
#include "ImguiWrapper.hpp"
#include "Window.hpp"
#include "UIButton.hpp"

struct Renderer;
struct Window;

struct UIImage : public UIElement {
	Name name;

	UIImage(Name name);
	void render(TiledRectangle renderArea, Renderer& renderer)override;
};
struct UITextBox : public UIElement {
	String* data;

	UITextBox(String& data);
	void render(TiledRectangle renderArea, Renderer& renderer) override;
};
struct UICheckBox : public UIElement {
	Boolean* data;
	UICheckBox(Boolean& data);
	void render(TiledRectangle renderArea, Renderer& renderer);
};
struct UIContainer : public UIElement {
	Boolean renderVertical = false;

	Vector<UIElement*> contents;

	UIContainer& vertical();
	UIContainer& horizontal();
	UIContainer& insert(UIElement& element);
	void render(TiledRectangle renderArea, Renderer& renderer) override;
	void renderInteractive(TiledRectangle renderArea, Renderer& renderer)override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers)override;
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
Window& window(String title, Area size, Bool decorated, WindowState state);
Window& window(String title, Area size, Bool decorated, WindowState state, UIElement& element);