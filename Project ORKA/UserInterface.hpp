
#pragma once

#include "Basics.hpp"
#include "GPUTexture.hpp"
#include "Window.hpp"
#include "UIElement.hpp"

struct Renderer;
struct Window;

struct Button : public UIElement {
	Bool* data;
	UIElement* content = nullptr;

	Button(Bool& data);
	Button& insert(UIElement* element);
	void render(TiledRectangle renderArea, Renderer& renderer)override;
};
struct UIImage : public UIElement {
	Name name;

	UIImage(Name name);
	void render(TiledRectangle renderArea, Renderer& renderer)override;;

};
struct TextBox : public UIElement {
	String* data;

	TextBox(String& data);
	void render(TiledRectangle renderArea, Renderer& renderer) override;
};
struct CheckBox : public UIElement {
	Boolean* data;
	CheckBox(Boolean& data);
	void render(TiledRectangle renderArea, Renderer& renderer);
};
struct Container : public UIElement {
	Boolean vertical = false;

	Vector<UIElement*> contents;

	Container& horizontal();
	Container& insert(UIElement& element);
	void render(TiledRectangle renderArea, Renderer& renderer) override;
};

struct UserInterface {
	void run();

	UIElement* currentlyActive = nullptr;

	List<UIImage> images;
	List<Window> windows;
	List<Button> buttons;
	List<TextBox> textBoxes;
	List<CheckBox> checkBoxes;
	List<Container> containers;

	UserInterface();
};

struct TestButton :public UIElement {
	Bool pressed = false;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers)override;;
	void render(TiledRectangle renderArea, Renderer& renderer)override {

		renderer.useShader("color");
		if (pressed) {
			renderer.uniforms().customColor(Color(1, 1, 0, 1));
		}
		else {
			PixelIDs ids = renderer.getIDsUnderCursor();
			if (ids.objectID == id) {
				renderer.uniforms().customColor(Color(1));
			}
			else {
				renderer.uniforms().customColor(Color(0.1, 0.1, 0.1, 1));
			}
		}

		renderer.screenSpace();
		renderer.uniforms().mMatrix(matrixFromLocationAndSize(Vec3(10, 10, 0), 100));

		renderer.renderMesh("button");
	};
	void renderInteractive(TiledRectangle renderArea, Renderer& renderer)override {
		renderer.useShader("idShader");
		renderer.screenSpace();
		renderer.uniforms().objectID(id);
		renderer.uniforms().mMatrix(matrixFromLocationAndSize(Vec3(10, 10, 0), 100));

		renderer.renderMesh("button");

	};
};

extern UserInterface ui;

Container& container();
UIImage& image(Name name);
Button& button(Bool& data);
CheckBox& checkBox(Bool& data);
TextBox& textBox(String& data);
Window& window(String title, Area size, Bool decorated, WindowState state);