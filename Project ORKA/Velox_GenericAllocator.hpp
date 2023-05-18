#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_AllocatorMisc.hpp"

class GenericAllocator {
public:
	using Traits = AllocTraits<true, 0>;

public:
	FORCEINLINE void* allocateBytes(const usize sz, const usize al = 1) const {
		// FIXME : this is a temp hack
		if (al == 1)
			return Utils::allocateBytes(sz);
		return Utils::allocateBytes(sz, al);
	}

	template <typename T>
	FORCEINLINE T* allocate(const usize count) {
		void* data = this->allocateBytes(sizeof(T) * count);
		return static_cast<T*>(data);
	}

	FORCEINLINE void* reallocateBytes(void* ptr, const usize sz, const usize al = 1) {
		return this->allocateBytes(sz, al);
	}

	template <typename T>
	FORCEINLINE T* reallocate(T* ptr, const usize count) {
		void* data = this->allocateBytes(sizeof(T) * count);
		return static_cast<T*>(data);
	}

	FORCEINLINE void freeMemory(void* ptr) noexcept { return Utils::freeMemory(ptr); }

	constexpr FORCEINLINE friend void swap(GenericAllocator& /*first*/, GenericAllocator& /*second*/) noexcept { }

private:
};
