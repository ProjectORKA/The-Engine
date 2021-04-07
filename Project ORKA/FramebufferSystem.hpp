
#pragma once

#include "Framebuffer.hpp"

struct FramebufferSystem {
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;

	UInt windowWidth = DEFAULT_WINDOW_WIDTH;
	UInt windowHeight = DEFAULT_WINDOW_HEIGHT;
	Bool needsUpdate = true;

	void create();
	void update();
	void destroy();
	void deselect();
	void add(UShort samples);
	void select(Index framebufferIndex);
	void updateSizes(UInt width, UInt height);

	Framebuffer& current();
};