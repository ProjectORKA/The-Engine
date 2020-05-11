
#pragma once

#include "Basics.hpp"

struct Mesh_Header {
	unsigned int version = 0;
	char meshname[100] = {};
	unsigned int renderType = 0;
	unsigned int vertexCount = 0;
	unsigned int vertexColorCount = 0;
	unsigned int textureCoordinateCount = 0;
	unsigned int indexCount = 0;
};

void loadString(String& string, Path path);