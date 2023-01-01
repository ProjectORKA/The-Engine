
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "BaseAllocator.hpp"

template<typename Allocator = BaseAllocator>
struct StaticAllocator{
	SizeT size;
	void* data = nullptr;
	void* head = nullptr;

	StaticAllocator(SizeT size) {
		data = malloc(size);
		if (data)this->size = size;
		else logError("Could not allocate own memory!");
	}
	void* allocate(SizeT size) {
		if ((head + size) < (data + this->size)) {
			void* oldHead = head;
			head = head + size;
			return oldHead;
		}
		else {
			logError("Could not provide memory, increase initial allocation!");
			return nullptr;
		}
	};
	~StaticAllocator() {
		free(data);
	}
};