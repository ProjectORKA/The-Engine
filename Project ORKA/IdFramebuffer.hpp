#pragma once

#include "Basics.hpp"
struct Window;
struct Renderer;
struct FramebufferSystem;

struct IdFramebuffer {
	UInt objectId = -1;
	UInt instanceId = -1;
	UInt triangleId = -1;
	Index framebufferId = 0;

	void read(Renderer& renderer) const;
	void draw(Renderer& renderer) const;
	void create(Renderer& renderer);
	void updateIdsUnderCursor(Window& window);
};
