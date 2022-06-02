
#pragma once

#include "Framebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Area framebufferSize = Area(1);
	Map<Name, Index> nametoID;
	Vector<Framebuffer> framebuffers;
	Index currentReadFramebufferIndex = 0;
	Index currentDrawFramebufferIndex = 0;

	void destroy();
	void deselect();
	void addGbuffer();
	void addIDBuffer();
	void update(Area area);
	Framebuffer& currentRead();
	Framebuffer& currentDraw();
	void create(Renderer& renderer, Area size);
	void read(Renderer& renderer, Index framebufferIndex);
	void draw(Renderer& renderer, Index framebufferIndex);
};