#pragma once

#include "Basics.hpp"

struct Message {
	Index type = 0;
	UInt byteSize = 0;
	Vector<Byte> data;
};
