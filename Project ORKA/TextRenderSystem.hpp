
#pragma once

#include "BitmapTextRenderSystem.hpp"
#include "GlyphTextRenderSystem.hpp"

struct TextRenderSystem {

	BitmapTextRenderSystem bitmapTextRenderSystem;
	GlyphTextRenderSystem glyphTextRenderSystem;
	//MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void create(Renderer& renderer);
	void render(String text, Float xPos, Float yPos, FontStyle style);
};