
#pragma once

//created by Blendurian

//this is supposed to load ttf files using freetype, but its not finished

#include "Basics.hpp"
#include "GPUTexture.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

struct CharData {
	IVec2   Size;       //size of glyph texture
	IVec2   Bearing;    //top left corner of glyph
	UInt	Advance;	//offset to next glyph
};

struct GPUFont {
	Bool loaded = false;
	GPUTexture textureAtlas;
	Vector<CharData> charData;

	void load();
};

struct GlyphTextRenderSystem {

	Vector<GPUFont> fonts;

	Index currentFont = 0;

	GlyphTextRenderSystem();
};