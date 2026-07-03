#pragma once
#include "Prototype.hpp"

struct ImageApproximationPrototype final : Prototype
{
	GPUTexture terrain;

	void action() override;

	void create(Renderer & r) override
	{
		Image img;
		img.load("blah.png");

		CpuTexture texture;
		texture.load(img, Filter::Nearest, Filter::Nearest, Wrapping::Repeat, "Terrain");
	}

	void destroy() override;
	void render(Renderer& r, const Player& player) override;
	void inputEvent(Window& window, InputEvent& input) override;
};
