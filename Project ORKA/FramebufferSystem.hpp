
#pragma once

#include "Framebuffer.hpp"
#include "IndexFramebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	IDFrameBuffer idFramebuffer;
	Vector<Framebuffer> framebuffers;
	Map<Name, Index> framebufferNames;
	Index currentFramebufferIndex = 0;
	Area framebufferSize = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);

	void destroy();
	void deselect();
	void update(Area area);
	Framebuffer& current();
	void add(Renderer& renderer);
	void create(Renderer& renderer);
	void use(Renderer& renderer, Index framebufferIndex);
};