#pragma once

#include "BitmapTextRenderSystem.hpp"

struct Renderer;
struct ResourceManager;
struct FontStyle;

struct TextRenderSystem
{
	// state
	Vec2      currentOffset = Vec2(0);
	FontStyle currentStyle  = fonts.debug;
	Alignment alignmentY    = Alignment::top;
	Alignment alignmentX    = Alignment::left;

	// SDFTextRenderSystem sdfTextRenderSystem;
	BitmapTextRenderSystem bitmapTextRenderSystem;
	// GlyphTextRenderSystem glyphTextRenderSystem;
	// MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void setOffset(Vec2 offset);
	void setStyle(FontStyle style);
	void alignText(Alignment x, Alignment y);
	void create(ResourceManager& resourceManager, Renderer& renderer);

	void render(ResourceManager& resourceManager, Renderer& renderer, const String& text);
	void render(ResourceManager& resourceManager, Renderer& renderer, const String& text, Vec2 position);
	void render(ResourceManager& resourceManager, Renderer& renderer, const String& text, FontStyle style);
	void render(ResourceManager& resourceManager, Renderer& renderer, const String& text, Vec2 position, FontStyle style);
private:
	void renderAdvanced(ResourceManager& resourceManager, Renderer& renderer, const String& text, Vec2 position, Alignment x, Alignment y, FontStyle style);
};
