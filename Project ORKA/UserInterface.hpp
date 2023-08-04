#pragma once

#include "UIElement.hpp"
#include "UIButton.hpp"
#include "Window.hpp"

struct Renderer;
struct Window;

struct UIImage final : UIElement
{
	Name name;

	UIImage(const Name& name);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct UITextBox final : UIElement
{
	String* data;

	UITextBox(String& data);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct UICheckBox final : UIElement
{
	Bool* data;

	UICheckBox(Bool& data);
	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct UIContainer final : UIElement
{
	Vector<UIElement*> contents;
	Bool               renderVertical = false;

	UIContainer& vertical();
	UIContainer& horizontal();
	UIContainer& insert(UIElement& element);
	void         update(Window& window) override;
	void         destroy(Window& window) override;
	void         inputEvent(Window& window, InputEvent input) override;
	void         create(ResourceManager& resourceManager, Window& window) override;
	void         render(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
	void         renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle renderArea) override;
};

struct UserInterface
{
	List<UIImage>     images;
	List<Window>      windows;
	List<UIButton>    buttons;
	List<UITextBox>   textBoxes;
	List<UICheckBox>  checkBoxes;
	List<UIContainer> containers;
	Bool              initialized     = false;
	UIElement*        currentlyActive = nullptr;

	void run();
	void create();

	UIButton&    button();
	UIContainer& container();
	UIImage&     image(Name name);
	UICheckBox&  checkBox(Bool& data);
	UITextBox&   textBox(String& data);
	Window&      window(const String& title, Area size, Bool decorated, Bool visible, WindowState state, ResourceManager& resourceManager);
	Window&      window(const String& title, Area size, Bool decorated, Bool visible, WindowState state, UIElement& element, ResourceManager& resourceManager);
};
