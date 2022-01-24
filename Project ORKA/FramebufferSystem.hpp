
#pragma once

#include "Framebuffer.hpp"
#include "IndexFramebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Area framebufferSize;
	IDFrameBuffer idFramebuffer;
	Vector<Framebuffer> framebuffers;
	Map<Name, Index> framebufferNames;
	Index currentFramebufferIndex = 0;

	void destroy();
	void deselect();
	void update(Area area);
	Framebuffer& current();
	void add(Renderer& renderer);
	void create(Renderer& renderer, Area size);
	void use(Renderer& renderer, Index framebufferIndex);
};