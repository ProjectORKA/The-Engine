#pragma once

#include <memory>

#define WHOLEPROGRAM_MAX_MEMORY (1024 * 1024)

struct Memory {
	void* memory = nullptr;

	Memory() {
		//memory = malloc(WHOLEPROGRAM_MAX_MEMORY);
	}

	void allocate() { };
	void free() { };
};

extern Memory memory;
