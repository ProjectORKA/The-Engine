
#include "TextRenderSystem.hpp"

#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::create(Renderer& renderer) {
	bitmapTextRenderSystem.create(renderer);
}

void TextRenderSystem::render(Renderer & renderer, String text, Vec2 position, FontStyle style) {
	bitmapTextRenderSystem.render(renderer, text, position, style);
}
