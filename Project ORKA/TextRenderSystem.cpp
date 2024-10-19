#include "TextRenderSystem.hpp"


#include "Renderer.hpp"

void TextRenderSystem::destroy(Renderer& renderer)
{
	bitmapTextRenderSystem.destroy(renderer);
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

void TextRenderSystem::create(Renderer& renderer)
{
	bitmapTextRenderSystem.create(renderer);
}

void TextRenderSystem::render(Renderer& renderer, const String& text)
{
	renderAdvanced(renderer, text, currentOffset, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(Renderer& renderer, const String& text, const Vec2 position)
{
	renderAdvanced(renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(Renderer& renderer, const String& text, const Float fontSize, const Float letterSpacing)
{
	renderAdvanced(renderer, text, currentOffset, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::render(Renderer& renderer, const String& text, const Vec2 position, const Float fontSize, const Float letterSpacing)
{
	renderAdvanced(renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}

void TextRenderSystem::setLetterSpacing(const Float letterSpacing)
{
	this->letterSpacing = letterSpacing;
}

void TextRenderSystem::renderAdvanced(Renderer& renderer, const String& text, const Vec2 position, const Alignment x, const Alignment y, const Float fontSize, const Float letterSpacing)
{
	alignText(x, y);
	setSize(fontSize);
	setLetterSpacing(letterSpacing);
	bitmapTextRenderSystem.render(renderer, text, position, alignmentX, alignmentY, fontSize, letterSpacing);
}
