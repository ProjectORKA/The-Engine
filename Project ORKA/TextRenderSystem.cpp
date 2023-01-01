
#include "TextRenderSystem.hpp"

#include "Engine.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::create(Engine& engine, Renderer& renderer) {
	bitmapTextRenderSystem.create(engine, renderer);
}

void TextRenderSystem::render(Engine& engine, Renderer & renderer, String text, Vec2 position, FontStyle style) {
	bitmapTextRenderSystem.render(engine, renderer, text, position, style);
}
