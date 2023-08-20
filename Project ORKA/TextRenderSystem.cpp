#include "TextRenderSystem.hpp"

#include "ResourceManager.hpp"
#include "Renderer.hpp"

void TextRenderSystem::destroy()
{
	bitmapTextRenderSystem.destroy();
}

void TextRenderSystem::setSize(const Float fontSize)
{
	this->fontSize = fontSize;
}

void TextRenderSystem::setOffset(const Vec2 offset)
{
	currentOffset = offset;
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
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position)
{
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Float fontSize, const Float letterSpacing)
{
	renderAdvanced(resourceManager, renderer, text, currentOffset, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position, const Float fontSize, const Float letterSpacing)
{
	renderAdvanced(resourceManager, renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::setLetterSpacing(const Float letterSpacing)
{
	this->letterSpacing = letterSpacing;
}

void TextRenderSystem::renderAdvanced(ResourceManager& resourceManager, Renderer& renderer, const String& text, const Vec2 position, const Alignment x, const Alignment y, const Float fontSize, const Float letterSpacing)
{
	alignText(x, y);
	setSize(fontSize);
	setLetterSpacing(letterSpacing);
	bitmapTextRenderSystem.render(resourceManager, renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}
