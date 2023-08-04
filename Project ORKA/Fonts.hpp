#pragma once

#include "Basics.hpp"

struct FontStyle
{
	Float absoluteSize  = 20.0f; // size in pixels
	Float relativeSize  = 1.0f;  // relative size of all fonts
	Float letterSpacing = 0.6f;

	FontStyle() = default;
	void     setSize(Float size);
	explicit FontStyle(Float absoluteSize);
};

struct Fonts
{
	FontStyle debug;
	FontStyle heading;
	FontStyle paragraph;

	Fonts();
	void setFontSize(Float size);
};

extern Fonts fonts;
