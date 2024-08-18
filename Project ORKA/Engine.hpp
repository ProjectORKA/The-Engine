#pragma once

#include "Basics.hpp"

struct Engine
{
	~Engine();
	void create(const Int argc, Char* argv[]) const;
};
