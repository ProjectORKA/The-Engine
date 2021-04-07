
#pragma once

//#include "ft2build.h"
//#include "freetype/freetype.h"
//#include "freetype/ftglyph.h"
//#include "freetype/ftoutln.h"
//#include "freetype/fttrigon.h"
//#include FT_FREETYPE_H
#include "Debug.hpp"
#include "Math.hpp"
#include "TextureSystem.hpp"

struct CPUFont {
	Name fontName = "defaultFont";
	Vector<CPUTexture> fontBitmaps;

	void create(Path path);
	void destroy();
};
struct GPUFont {
	Float height;
	Vector<UInt> gpuTextureIDs;

	void destroy();
	void render(String text, Float x, Float y);
	void create(CPUFont cpuFont, TextureSystem & textureSystem, UInt height);
};

struct TextRenderSystem {
	Vector<GPUFont> fonts;
	TextureSystem* textureSystem = nullptr;
	Index currentFontID = 0;

	void destroy();
	void select(Index fontID);
	void create(TextureSystem& textureSystem);
	void render(String text, Float x, Float y);
	GPUFont& currentFont();
};