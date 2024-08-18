#pragma once

#include "Renderer.hpp"

struct Window;
struct InputEvent;

struct Prototype
{
	virtual void action() = 0;
	virtual void create() = 0;
	virtual void destroy() = 0;
	virtual      ~Prototype() = default;
	virtual void render(Renderer& r, const Player& player) = 0;
	virtual void inputEvent(Window& window, InputEvent input) = 0;
};
