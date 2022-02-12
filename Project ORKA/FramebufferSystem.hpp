
#pragma once

#include "Framebuffer.hpp"
#include "IndexFramebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Area framebufferSize;
	Vector<Framebuffer *> framebuffers;
	Map<Name, Index> framebufferNames;
	Index currentFramebufferIndex = 0;

	IDFrameBuffer * idFbPtr = nullptr;

	void destroy();
	void deselect();
	void update(Area area);
	Framebuffer& current();
	IDFrameBuffer& idFramebuffer();
	void add(Framebuffer * framebuffer);
	void create(Renderer& renderer, Area size);
	void use(Renderer& renderer, Index framebufferIndex);
};