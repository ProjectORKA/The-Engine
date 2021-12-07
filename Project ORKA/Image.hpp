
#pragma once

#include "Basics.hpp"

struct Image{
	Byte* pixels = nullptr;
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int bitcount = 8;
};