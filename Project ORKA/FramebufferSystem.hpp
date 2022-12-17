
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
	void drawToWindow();
	void update(Area area);
	Framebuffer& currentRead();
	Framebuffer& currentDraw();
	void addGbuffer(Name name);
	void addIDBuffer(Name name);
	void addFrameBuffer(Name name);
	void read(Name name);
	void draw(Name name);
	void create(Renderer& renderer, Area size);
	void read(Index framebufferIndex);
	void draw(Index framebufferIndex);
};