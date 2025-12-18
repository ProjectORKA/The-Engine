#pragma once

#include "Basics.hpp"

struct Engine
{
	Bool useJobSystem = true;

	~Engine();
	void create(Int argc, Char* argv[], Bool useJobSystem);
};
