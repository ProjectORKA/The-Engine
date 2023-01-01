
#pragma once

#include "BitmapTextRenderSystem.hpp"

struct Renderer;
struct Engine;
struct FontStyle;

struct TextRenderSystem {
	//SDFTextRenderSystem sdfTextRenderSystem;
	BitmapTextRenderSystem bitmapTextRenderSystem;
	//GlyphTextRenderSystem glyphTextRenderSystem;
	//MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void create(Engine& engine, Renderer& renderer);
	void render(Engine& engine, Renderer& renderer, String text, Vec2 position, FontStyle style);
};