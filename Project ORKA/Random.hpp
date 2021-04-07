
#include "Basics.hpp"
#include "Threading.hpp"

#pragma once

struct RandomNumberGenerator {
	unsigned short a = 0;
	unsigned short b = 0;
	unsigned short c = 0;

	ULL randomULL();
	RandomNumberGenerator();
};
