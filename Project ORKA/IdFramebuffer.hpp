
#pragma once

#include "Basics.hpp"
struct Window;
struct Renderer;
struct FramebufferSystem;

struct IdFramebuffer {
	UInt objectID = -1;
	UInt instanceID = -1;
	UInt triangleID = -1;
	Index framebufferID = 0;

	void read(Renderer& renderer);
	void draw(Renderer& renderer);
	void create(Renderer & renderer);
	void updateIdsUnderCursor(Window& window);
};