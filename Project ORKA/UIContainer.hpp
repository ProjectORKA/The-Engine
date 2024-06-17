#pragma once
#include "UIElement.hpp"

struct UIContainer : UIElement
{
	UIContainer&       vertical();
	UIContainer&       horizontal();
	[[nodiscard]] Bool isEmpty() const;

	void add(UIElement& element);
	void drop(const UIElement* element);
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, const TiledRectangle renderArea) override;
	void renderInteractive(Window& window, const TiledRectangle renderArea) override;

private:
	Vector<UIElement*> contents;
	Bool               renderVertical = false;
};
