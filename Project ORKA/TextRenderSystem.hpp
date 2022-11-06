
#pragma once

#include "BitmapTextRenderSystem.hpp"
#include "GlyphTextRenderSystem.hpp"
//#include "SDFTextRenderSystem.hpp"

struct Renderer;

struct TextRenderSystem {
	//SDFTextRenderSystem sdfTextRenderSystem;
	BitmapTextRenderSystem bitmapTextRenderSystem;
	//GlyphTextRenderSystem glyphTextRenderSystem;
	//MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void create(Renderer& renderer);
	void render(Renderer& renderer, String text, Vec2 position, FontStyle style);
};