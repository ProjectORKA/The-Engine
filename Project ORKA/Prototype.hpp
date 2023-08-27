#pragma once

struct Prototype
{
	virtual void action() = 0;
	virtual void create() = 0;
	virtual void render(Renderer& r, ResourceManager& rm, const Player& player) = 0;
};
