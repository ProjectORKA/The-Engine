
#include "Fonts.hpp"

Fonts fonts;

void Fonts::setFontSize(Float size) {
	debug.setSize(size);
	heading.setSize(size);
	paragraph.setSize(size);
}

Fonts::Fonts() {
	debug.relativeSize = 4.0f;
	heading.relativeSize = 3.0f;
	setFontSize(20);
}

FontStyle::FontStyle(Float absoluteSize) {
	this->absoluteSize = absoluteSize;
}

void FontStyle::setSize(Float size) {
	absoluteSize = size * relativeSize;
}
