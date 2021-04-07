#include "TextRenderSystem.hpp"

//CPUFont
void CPUFont::create(Path path)
{
	//check file extension
	if (path.extension() != ".ttf") logError("Attempting to load non ttf font!");


}
void CPUFont::destroy()
{
}

//GPUFont
void GPUFont::destroy() {
	gpuTextureIDs.clear();
}
void GPUFont::render(String text, Float x, Float y)
{
}
void GPUFont::create(CPUFont cpuFont, TextureSystem & textureSystem, UInt height) {

	//FT_Set_Char_Size(cpuFont.freeTypeFace, height * 64, height * 64, 96, 96);

	//this->height = height;

	//gpuTextureIDs.clear();
	//
	//CPUTexture cpuTexture;
	//cpuTexture.channels = 2;
	//cpuTexture.dataType = dataTypeByte;
	//cpuTexture.filter = nearest;//[TODO] check if okay
	//cpuTexture.wrapping = repeat;
	//cpuTexture.multisampling = 0;

	//for (int i = 0; i < 128; i++) {

	//	FT_BitmapGlyph bitmapGlyph = generateBitmapForFace(cpuFont.freeTypeFace, i);
	//	FT_Bitmap& bitmap = bitmapGlyph->bitmap;

	//	cpuTexture.name = "fontCharacter";
	//	cpuTexture.height = nextPowerOfTwo(bitmap.rows);
	//	cpuTexture.width = nextPowerOfTwo(bitmap.width);
	//	cpuTexture.bytePixels = bitmap.buffer;


	//	textureSystem.add(cpuTexture);
	//	gpuTextureIDs.push_back(textureSystem.currentTextureID);
	//}


	//for (unsigned char i = 0; i < 128; i++) {
	//	make_dlist(cpuFont.freeTypeFace, i, list_base, &textureIDs.front());
	//}


}

//TextRenderSystem
void TextRenderSystem::destroy()
{
	for (GPUFont f : fonts) {
		f.destroy();
	}
	textureSystem = nullptr;
}
void TextRenderSystem::create(TextureSystem& textureSystem)
{
	this->textureSystem = &textureSystem;

	fonts.emplace_back();
	CPUFont cpuFont;
	cpuFont.create("Data/fonts/nasalization-rg.ttf");
	fonts.back().create(cpuFont, textureSystem, 32);
}
void TextRenderSystem::render(String text, Float x, Float y)
{
	Vector<Char> characters(text.begin(), text.end());
	for (Char c : characters) {
		textureSystem->use(currentFont().gpuTextureIDs[c]);
	}
}
GPUFont& TextRenderSystem::currentFont()
{
	return fonts[currentFontID];
}

void TextRenderSystem::select(Index fontID)
{
	if (fonts.size() > fontID) {
		currentFontID = fontID;
	}
	else {
		logError("Requesting font that doesn't exist!");
	}
}
