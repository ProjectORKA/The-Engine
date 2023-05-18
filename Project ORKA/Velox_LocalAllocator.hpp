#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_AllocatorMisc.hpp"

template <usize SIZE = 4096>
class LocalAllocator {
public:
	using Traits = AllocTraits<true, SIZE>;

public:
	constexpr FORCEINLINE void* allocateBytes(const usize sz, const usize al = 1) {
		// FIXME : this doesn't take into account alignement
		const usize remaining = SIZE - m_Reserved;

		if (sz < remaining) {
			m_Reserved += sz;
			m_LatestSize = sz;
			return m_Buffer;
		}

		return Utils::allocateBytes(sz, al);
	}

	template <typename T>
	constexpr FORCEINLINE T* allocate(const usize count) {
		void* data = this->allocateBytes(sizeof(T) * count);
		return static_cast<T*>(data);
	}

	constexpr FORCEINLINE void* reallocateBytes(void* ptr, const usize sz, usize al = 1) {
		const uint8* oldPtr = m_Buffer - m_LatestSize;
		const usize remaining = SIZE - m_Reserved;
		const usize addition = sz - m_LatestSize;

		if (oldPtr == ptr && addition < remaining) {
			m_LatestSize = sz;
			return ptr;
		}

		return Utils::allocateBytes(sz);
	}

	template <typename T>
	constexpr FORCEINLINE T* reallocate(T* ptr, const usize count) {
		void* data = this->reallocateBytes(ptr, sizeof(T) * count);
		return static_cast<T*>(data);
	}

	constexpr FORCEINLINE void freeMemory(void* ptr) noexcept {
		if (ptr < m_Buffer && ptr >= m_Buffer + SIZE)
			Utils::freeMemory(ptr);
	}

	constexpr LocalAllocator() = default;
	constexpr LocalAllocator(LocalAllocator&& other) = delete;
	constexpr LocalAllocator& operator=(LocalAllocator&& other) = delete;
	constexpr friend void swap(LocalAllocator& first, LocalAllocator& second) noexcept = delete;

private:
	alignas(16) uint8 m_Buffer[SIZE];
	usize m_Reserved = 0;
	usize m_LatestSize = 0;
};
