#pragma once

//stb image
#define STBI_ASSERT(x)
#include "stb_image.h"

#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Debug.hpp"

enum Wrapping
{
#ifdef GRAPHICS_API_OPENGL
	repeat = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	clamped = GL_CLAMP_TO_EDGE,
	border = GL_CLAMP_TO_BORDER
#endif
};
enum Filter {
#ifdef GRAPHICS_API_OPENGL
	nearest = GL_NEAREST,
	nearestMM = GL_NEAREST_MIPMAP_NEAREST,
	linear = GL_LINEAR,
	linearMM = GL_LINEAR_MIPMAP_LINEAR
#endif
};
enum DataType {
#ifdef GRAPHICS_API_OPENGL
	dataTypeByte = GL_UNSIGNED_BYTE,
	dataTypeFloat = GL_FLOAT
#endif
};

struct CPUTexture {
	String name = "empty";
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int dataType = dataTypeByte;
	union {
		void* pixels;
		Byte* bytePixels;
		Float* floatPixels;
	};
	Int filter = linearMM;
	Int wrapping = repeat;
	Bool loaded = false;

	void loadRGB(Path path, String name);
	void loadRGBA(Path path, String name);
	void loadHDR(Path path, String name);
	void unload();

	~CPUTexture();
};
struct GPUTexture {
	Index textureID;
	Int filter = linearMM;
	Int wrapping = repeat;
	Bool loaded = false;

	void load(CPUTexture& cpuTexture);
	void render();
	void unload();
};

struct TextureSystem {
	Map<String, Index> textureNames;

	Vector<CPUTexture> cpuTextures;
	Vector<GPUTexture> gpuTextures;

	void create();
	void add(CPUTexture& cpuTexture);
	void render(String name);
	void render(Index textureID);
	void destroy();
};

void loadAllTextures(TextureSystem & textureSystem);