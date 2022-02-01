
#pragma once

#include "Basics.hpp"

struct GPUBuffer {
	Index bufferID;
	Bool loaded = false;
	void use(Int location);
	void create(Vector<Vec4>& data, Int location);
};