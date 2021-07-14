
#pragma once

#include "Framebuffer.hpp"

struct FramebufferSystem {
	Bool needsUpdate = true;
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;
	Area currentFramebufferSize = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);

	void add();
	void create();
	void update();
	void destroy();
	void deselect();
	void updateSizes(Area area);
	void select(Index framebufferIndex);
	Framebuffer& current();
};