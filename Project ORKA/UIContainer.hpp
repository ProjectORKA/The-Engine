#pragma once
#include "UIElement.hpp"

struct UIContainer : UIElement
{
	Vector<UIElement*> contents;
	Bool               renderVertical = false;

	UIContainer& vertical()
	{
		renderVertical = true;
		return *this;
	}

	UIContainer& horizontal()
	{
		renderVertical = false;
		return *this;
	}

	virtual UIContainer& insert(UIElement& element)
	{
		contents.push_back(&element);
		return *this;
	}

	void update(Window& window) override
	{
		for(UIElement* c : contents) c->update(window);
	}

	void destroy(Window& window) override
	{
		for(UIElement* c : contents) c->destroy(window);
	}

	void inputEvent(Window& window, const InputEvent input) override
	{
		for(UIElement* c : contents) c->inputEvent(window, input);
	}

	void create(Window& window) override
	{
		for(UIElement* c : contents) c->create(window);
	}

	void render(Window& window, const TiledRectangle renderArea) override
	{
		for(UInt i = 0; i < contents.size(); i++)
		{
			TiledRectangle a = renderArea;
			if(renderVertical)
			{
				a.size.y /= contents.size();
				a.position.y -= a.size.y * i;
			}
			else
			{
				a.size.x /= contents.size();
				a.position.x += a.size.x * i;
			}
			contents[i]->render(window, a);
		}
	}

	void renderInteractive(Window& window, const TiledRectangle renderArea) override
	{
		for(UInt i = 0; i < contents.size(); i++)
		{
			TiledRectangle a = renderArea;
			if(renderVertical)
			{
				a.size.y /= contents.size();
				a.position.y += a.size.y * i;
			}
			else
			{
				a.size.x /= contents.size();
				a.position.x += a.size.x * i;
			}
			contents[i]->renderInteractive(window, a);
		}
	}

	void drop(const UIElement* element)
	{
		for(Int i = 0; i < contents.size(); i++){

			if(contents[i] == element) contents.erase(contents.begin() + i);
		}
	}
};
