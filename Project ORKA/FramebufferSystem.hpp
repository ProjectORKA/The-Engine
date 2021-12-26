
#pragma once

#include "Framebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;
	Area framebufferSize = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);

	void destroy();
	void deselect();
	void update(Area area);
	void add(Renderer& renderer);
	void create(Renderer& renderer);
	void use(Renderer& renderer, Index framebufferIndex);
	Framebuffer& current();
};