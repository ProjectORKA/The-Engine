#pragma once

struct Renderer;

struct Renderable {
	virtual void render(Renderer& renderer);
};