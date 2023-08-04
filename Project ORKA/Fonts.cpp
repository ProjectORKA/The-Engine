#include "Fonts.hpp"

Fonts fonts;

void Fonts::setFontSize(const Float size)
{
	debug.setSize(size);
	heading.setSize(size);
	paragraph.setSize(size);
}

Fonts::Fonts()
{
	debug.relativeSize   = 4.0f;
	heading.relativeSize = 3.0f;
	setFontSize(20);
}

FontStyle::FontStyle(const Float absoluteSize)
{
	this->absoluteSize = absoluteSize;
}

void FontStyle::setSize(const Float size)
{
	absoluteSize = size * relativeSize;
}
