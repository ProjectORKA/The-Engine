
#pragma once

#include "BitmapTextRenderSystem.hpp"

struct TextRenderSystem {

	BitmapTextRenderSystem bitmapTextRenderSystem;

	void destroy();
	void create(Renderer& renderer);
	void render(String text, Float xPos, Float yPos, FontStyle style);
};