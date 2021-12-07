#pragma once

#include "Basics.hpp"

struct FontStyle {
	Float absoluteSize = 20;	//size in pixels
	Float relativeSize = 1;		//relative size of all fonts
	Float letterSpacing = 0.5;

	void setSize(Float size);
};

struct Fonts {
	FontStyle debug;
	FontStyle heading;
	FontStyle paragraph;

	Fonts();
	void setFontSize(Float size);
};

extern Fonts fonts;