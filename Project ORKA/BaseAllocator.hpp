
#pragma once

#include "Basics.hpp"

struct BaseAllocator {
	void* allocate(SizeT size) {
		malloc(size);
	}
	void deallocate(void * data){
		free(data);
	}
};