#include "TextRenderSystem.hpp"

#include "ResourceManager.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy()
{
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::setOffset(const Vec2 offset)
{
	currentOffset = offset;
}

void TextRenderSystem::setStyle(const FontStyle style)
{
	currentStyle = style;
}

void TextRenderSystem::alignText(const Alignment x, const Alignment y)
{
	alignmentX = x;
	alignmentY = y;
}

void TextRenderSystem::create(ResourceManager& resourceManager, Renderer& renderer)
{
	bitmapTextRenderSystem.create(resourceManager, renderer);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text)
{
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, currentStyle);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position)
{
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, currentStyle);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const FontStyle style)
{
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, style);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position, const FontStyle style)
{
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, style);
}

void TextRenderSystem::renderAdvanced(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position, const Alignment x, const Alignment y, const FontStyle style)
{
	alignText(x, y);
	setStyle(style);
	bitmapTextRenderSystem.render(resourceManager, renderer, text, position, alignmentX, alignmentY, style);
}
