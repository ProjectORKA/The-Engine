#pragma once

#include "Renderer.hpp"
#include "ResourceManager.hpp"

struct Prototype
{
	virtual void action() = 0;
	virtual void create() = 0;
	virtual void render(Renderer& r, const Player& player) = 0;
};
