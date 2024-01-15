#pragma once

#include "BitmapTextRenderSystem.hpp"

struct Renderer;
struct ResourceManager;

struct TextRenderSystem
{
	// state
	Float     letterSpacing = 0.6f;
	Float     fontSize      = 20.0f;
	Vec2      currentOffset = Vec2(0);
	Alignment alignmentY    = Alignment::top;
	Alignment alignmentX    = Alignment::left;

	// SDFTextRenderSystem sdfTextRenderSystem;
	BitmapTextRenderSystem bitmapTextRenderSystem;
	// GlyphTextRenderSystem glyphTextRenderSystem;
	// MeshTextrenderSystem meshTextRenderSystem;

	void destroy();
	void setOffset(Vec2 offset);
	void setSize(Float fontSize);
	void alignText(Alignment x, Alignment y);
	void setLetterSpacing(Float letterSpacing);
	void create(Renderer& renderer);

	void render(Renderer& renderer, const String& text);
	void render(Renderer& renderer, const String& text, Vec2 position);
	void render(Renderer& renderer, const String& text, Float fontSize, Float letterSpacing);
	void render(Renderer& renderer, const String& text, Vec2 position, Float fontSize, Float letterSpacing);
private:
	void renderAdvanced(Renderer& renderer, const String& text, Vec2 position, Alignment x, Alignment y, Float fontSize, Float letterSpacing);
};
