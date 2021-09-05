
#include "Fonts.hpp"

Fonts fonts;

void Fonts::setFontSize(Float size) {
	debug.setSize(size);
	heading.setSize(size);
	paragraph.setSize(size);
}

Fonts::Fonts() {
	debug.relativeSize = 4;
	heading.relativeSize = 3;
	setFontSize(20);
}

void FontStyle::setSize(Float size) {
	absoluteSize = size * relativeSize;
}
