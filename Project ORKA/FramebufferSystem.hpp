
#pragma once

#include "Framebuffer.hpp"

struct FramebufferSystem {
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;
	Area framebufferSize = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);

	void add();
	void create();
	void destroy();
	void deselect();
	void update(Area area);
	void use(Index framebufferIndex);
	Framebuffer& current();
};