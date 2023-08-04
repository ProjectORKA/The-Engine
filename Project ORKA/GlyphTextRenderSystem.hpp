#pragma once

// created by Blendurian

// this is supposed to load ttf files using freetype, but its not finished

#include "Basics.hpp"
#include "GPUTexture.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

struct CharData
{
	IVec2 size; // size of glyph texture
	IVec2 bearing; // top left corner of glyph
	UInt  advance; // offset to next glyph
};

struct GPUFont
{
	Vector<CharData> charData;
	GPUTexture       textureAtlas;
	Bool             loaded = false;

	void load();
};

struct GlyphTextRenderSystem
{
	Vector<GPUFont> fonts;
	Index           currentFont = 0;

	GlyphTextRenderSystem();
};
