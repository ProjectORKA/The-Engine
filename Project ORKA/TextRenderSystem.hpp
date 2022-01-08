
#pragma once

#include "BitmapTextRenderSystem.hpp"
#include "GlyphTextRenderSystem.hpp"

struct Renderer;

struct TextRenderSystem {

	BitmapTextRenderSystem bitmapTextRenderSystem;
	GlyphTextRenderSystem glyphTextRenderSystem;
	//MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void create(Renderer& renderer);
	void render(Renderer& renderer, String text, Vec2 position, FontStyle style);
};