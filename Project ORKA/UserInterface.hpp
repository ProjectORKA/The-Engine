
#pragma once

#include "Renderer.hpp"
#include "GameSystem.hpp"

extern Index newUIelementID;

struct UserInterface;

extern UserInterface ui;

struct UIElement {
	Index id = newUIelementID++;
	//TiledRectangle screenArea;

	virtual void render(Renderer& renderer) {};
	virtual void mouseIsMoving(Window& window, IVec2 position) {};
	virtual void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {};
	virtual void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {};

	~UIElement();
};
struct UIImage : public UIElement {
	GPUTexture* image;

	void render(Renderer& renderer) {

	};

	UIImage(GPUTexture& image) {
		this->image = &image;
	};

};
struct TextBox : public UIElement {
	String* data;

	void render(Renderer& renderer);
	TextBox(String& data);
};
struct CheckBox : public UIElement {
	Boolean* data;
	void render(Renderer& renderer);
	CheckBox(Boolean& data);
};
struct Container : public UIElement {
	Boolean vertical = false;

	Vector<UIElement*> contents;

	void render(Renderer& renderer);
	Container& insert(UIElement* element);
	Container& horizontal();
};
struct Button : public UIElement {
	Bool* data;
	UIElement* content = nullptr;

	void render(Renderer& renderer);

	Button(Bool& data) {
		this->data = &data;
	}

	Button& insert(UIElement* element);
};
struct GameView : public UIElement {
	using UIElement::UIElement;

	Index gameID = 0;

	void render(Renderer& renderer);
	void mouseIsMoving(Window& window, IVec2 position) override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;

	GameView(Index gameID);
};

struct UserInterface {
	void run();

	UIElement* currentlyActive = nullptr;

	List<Window> windows; //[TODO] check if right container

	//Elements
	Vector<Button> buttons;
	Vector<TextBox> textBoxes;
	Vector<GameView> gameViews;
	Vector<CheckBox> checkBoxes;
	Vector<Container> containers;

	UserInterface();
};

GameView& gameView();
Container& container();
Button & button(Bool& data);
TextBox& textBox(String& data);
CheckBox& checkBox(Bool& data);
GameView& gameView(Index gameID);
Window& window(String title, UIElement * element);


void exampleCode();