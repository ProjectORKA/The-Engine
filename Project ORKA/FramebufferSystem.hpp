
#pragma once

#include "Framebuffer.hpp"

struct Renderer;

struct FramebufferSystem {
	Area windowSize = Area(1);
	Map<Name, Index> nametoID;
	Vector<Framebuffer> framebuffers;
	Index currentReadFramebufferIndex = 0;
	Index currentDrawFramebufferIndex = 0;

	void destroy();
	void deselect();
	void drawToWindow();
	void update(Area area);
	Framebuffer& currentRead();
	Framebuffer& currentDraw();
	void setAsTexture(Name name, Index slot) {
		framebuffers[nametoID[name]].setAsTexture(slot);
	};
	Framebuffer& addGbuffer(Name name, Float relativeSize = 1.0f);
	Framebuffer& addIDBuffer(Name name, Float relativeSize = 1.0f);
	Framebuffer& addFrameBuffer(Name name, Float relativeSize = 1.0f);
	void read(Renderer& renderer, Name name);
	void draw(Renderer& renderer, Name name);
	void create(Renderer& renderer, Area size);
	void read(Renderer& renderer, Index framebufferIndex);
	void draw(Renderer& renderer, Index framebufferIndex);
	void blit() { //check if works
		Area ss = currentRead().size;
		Area ds = currentDraw().size;
		apiBlitFramebuffer(ss.x, ss.y,ds.x,ds.y, GL_LINEAR);
	}
};