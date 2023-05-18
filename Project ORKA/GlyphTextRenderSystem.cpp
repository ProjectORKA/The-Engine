#include "GlyphTextRenderSystem.hpp"

void GPUFont::load() {
	if (loaded) { logError("Font already loaded!"); }
	else {
		FT_Library ft;

		//initialize freetype
		if (FT_Init_FreeType(&ft)) { logError("Could not initialize FreeType!"); }
		else {
			logDebug("Initialized Freetype!");

			//load font
			FT_Face face;
			if (FT_New_Face(ft, "data/fonts/minimal.ttf", 0, &face)) logError("Failed to load font!");

			//set desired size
			FT_Set_Pixel_Sizes(face, 0, 32);

			//load characters
			for (Int c = 0; c < 256; c++) {
				if (FT_Load_Char(face, c, FT_LOAD_RENDER)) logError("Failed to load Char!");
			}

			//do more stuff

			loaded = true;

			FT_Done_FreeType(ft);
			logDebug("Terminated Freetype!");
		}
	}
}

GlyphTextRenderSystem::GlyphTextRenderSystem() {
	fonts.emplace_back();
	fonts.back().load();
}
