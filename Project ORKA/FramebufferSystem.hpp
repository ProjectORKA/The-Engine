#pragma once

#include "Framebuffer.hpp"

struct Renderer;

struct FramebufferSystem
{
	Area windowSize = Area(1);
	Map<Name, Index> nametoId;
	Vector<Framebuffer> framebuffers;
	Index currentReadFramebufferIndex = 0;
	Index currentDrawFramebufferIndex = 0;

	void destroy() const;
	static void deselect();
	void drawToWindow() const;
	void update(Area area);
	Framebuffer& currentRead();
	Framebuffer& currentDraw();

	void setAsTexture(const Name& name, const Index slot);;
	Framebuffer& addGBuffer(const Name& name, Float relativeSize = 1.0f);
	Framebuffer& addIdBuffer(const Name& name, Float relativeSize = 1.0f);
	Framebuffer& addFramebuffer(const Name& name, Float relativeSize = 1.0f);
	void read(Renderer& renderer, const Name& name);
	void draw(Renderer& renderer, const Name& name);
	void create(Renderer& renderer, Area size);
	void read(Renderer& renderer, Index framebufferIndex);
	void draw(Renderer& renderer, Index framebufferIndex);

	void blit();
};