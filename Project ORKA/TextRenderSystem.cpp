
#include "TextRenderSystem.hpp"

#include "ResourceManager.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy() {
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::setStyle(FontStyle style) {
	currentStyle = style;
}

void TextRenderSystem::setOffset(Vec2 offset) {
	currentOffset = offset;
}

void TextRenderSystem::alignText(Alignment x, Alignment y) {
	alignmentX = x;
	alignmentY = y;
}

void TextRenderSystem::create(ResourceManager& resourceManager, Renderer& renderer) {
	bitmapTextRenderSystem.create(resourceManager, renderer);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, String text, FontStyle style) {
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, style);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, String text) {
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, currentStyle);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, String text, Vec2 position) {
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, currentStyle);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, String text, Vec2 position, FontStyle style) {
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, style);
}

void TextRenderSystem::renderAdvanced(ResourceManager& resourceManager, Renderer & renderer, String text, Vec2 position, Alignment x, Alignment y, FontStyle style) {
	alignText(x, y);
	setStyle(style);
	bitmapTextRenderSystem.render(resourceManager, renderer, text, position, alignmentX,alignmentY, style);
}
