
#pragma once

#include "Basics.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H  

struct CharData {
	IVec2   Size;       //size of glyph texture
	IVec2   Bearing;    //top left corner of glyph
	UInt	Advance;	//offset to next glyph
};

struct GPUFont {
	GPUTexture textureAtlas;
	Vector<CharData> charData;

	void load() {
		FT_Library ft;

		//initialize freetype
		if (FT_Init_FreeType(&ft)) logError("Could not initialize FreeType!");
		else logDebug("Initialized Freetype!");

		//load font
		FT_Face face;
		if (FT_New_Face(ft, "data/fonts/minimal.ttf", 0, &face)) logError("Failed to load font!");

		//set desired size
		FT_Set_Pixel_Sizes(face, 0, 32);

		//load characters
		for (Int c = 0; c < 256; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) logError("Failed to load Char!");
		}

		face;

	}
};

struct GlyphTextRenderSystem {

	Vector<GPUFont> fonts;

	Index currentFont = 0;

	GlyphTextRenderSystem() {
		fonts.emplace_back();
		fonts.back().load();
	}
};