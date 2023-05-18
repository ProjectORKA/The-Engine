#pragma once

#include "Basics.hpp"

struct BaseAllocator {
	void* allocate(const SizeT size) { malloc(size); }
	void deallocate(void* data) { free(data); }
};
