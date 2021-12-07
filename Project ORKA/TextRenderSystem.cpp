
#include "TextRenderSystem.hpp"

#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::create(Renderer& renderer) {
	bitmapTextRenderSystem.create(renderer);
}

void TextRenderSystem::render(String text, Float xPos, Float yPos, FontStyle style) {
	bitmapTextRenderSystem.render(text, xPos, yPos, style);
}
