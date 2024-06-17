#pragma once

#include "Renderer.hpp"

struct Prototype
{
	virtual ~Prototype() = default;
	virtual void action() = 0;
	virtual void create() = 0;
	virtual void destroy() = 0;
	virtual void render(Renderer& r, const Player& player) = 0;
};
