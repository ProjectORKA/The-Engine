#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <type_traits>
#include <cstring>
#include <utility>
#include <new>
#include <algorithm>
#include "Velox_Common.hpp"
#include "Velox_UtilityConcepts.hpp"

namespace Utils {
	template <typename integral>
	FORCEINLINE constexpr bool isAligned(integral x, usize a) noexcept { return (x & (integral(a) - 1)) == 0; }

	template <typename integral>
	FORCEINLINE constexpr integral alignUp(integral x, usize a) noexcept {
		return integral((x + (integral(a) - 1)) & ~integral(a - 1));
	}

	template <typename integral>
	FORCEINLINE constexpr integral alignDown(integral x, const usize a) noexcept {
		return integral(x & ~integral(a - 1));
	}

	FORCEINLINE void* allocateBytes(const usize sz) { return operator new(sz); }

	FORCEINLINE void* allocateBytes(const usize sz, usize al) {
		return operator new(sz, static_cast<std::align_val_t>(al));
	}

	template <typename T>
	FORCEINLINE T* allocate(const usize sz) {
		// return static_cast<T*>(::operator new(sz * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
		return static_cast<T*>(operator new(sz * sizeof(T)));
	}

	FORCEINLINE void freeMemory(void* ptr) noexcept {
		if (ptr)
			operator delete(ptr);
		//delete ptr;
	}

#if defined(COMPILER_MSVC) || __cplusplus >= 202002L
	// POD TYPES:
	template <POD T>
	FORCEINLINE constexpr void copy(T* dst, const T* src, const usize count = 1) noexcept {
		std::memcpy(dst, src, count * sizeof(T));
	}

	template <POD T>
	FORCEINLINE constexpr void copyConstruct(T* dst, const T* src, usize count = 1) noexcept {
		Utils::copy(dst, src, count);
	}

	template <POD T>
	FORCEINLINE constexpr void move(T* dst, T* src, usize count = 1) noexcept { Utils::copy(dst, src, count); }

	template <POD T>
	FORCEINLINE constexpr void moveForward(T* dst, T* src, ssize start, ssize end) noexcept {
		Utils::copy(dst + start, src + end, end - start);
	}

	template <POD T>
	FORCEINLINE constexpr void moveBackward(T* dst, T* src, const ssize start, ssize end) noexcept {
		std::memmove(dst + end, src + end, (start - end + 1) * sizeof(T));
	}

	template <POD T>
	FORCEINLINE constexpr void moveConstruct(T* dst, T* src, usize count = 1) noexcept { Utils::copy(dst, src, count); }

	template <POD T>
	FORCEINLINE constexpr void moveConstructForward(T* dst, T* src, ssize start, const ssize end) noexcept {
		Utils::copy(dst + start, src + start, end - start);
	}

	template <POD T>
	FORCEINLINE constexpr void moveConstructBackward(T* dst, T* src, ssize start, ssize end) noexcept {
		Utils::moveBackward(dst, src, start, end);
	}

	template <POD T>
	FORCEINLINE constexpr void memSet(T* dst, const T& src, usize count = 1) noexcept {
		std::fill(dst, dst + count, src);
	}

	template <POD T>
	FORCEINLINE constexpr bool memCmp(const T* s1, const T* s2, const usize count = 1) {
		return std::memcmp(s1, s2, sizeof(T) * count) == 0;
	}

	template <POD T>
	FORCEINLINE constexpr void destroy(T* /*ptr*/, usize /*count = 1*/) noexcept { }

	template <POD T>
	FORCEINLINE constexpr void free(T* ptr, usize /*count = 1*/) noexcept { Utils::freeMemory(ptr); }
#endif

	// NON POD TYPES :
	template <typename T>
	FORCEINLINE constexpr void copy(T* dst, const T* src, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) { dst[i] = T(src[i]); }
	}

	template <typename T>
	FORCEINLINE constexpr void copyConstruct(T* dst, const T* src, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) { new(&dst[i]) T(src[i]); }
	}

	template <typename T>
	FORCEINLINE constexpr void move(T* dst, T* src, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template <typename T>
	FORCEINLINE constexpr void moveForward(T* dst, T* src, const ssize start, const ssize end) noexcept {
		for (ssize i = start; i < end; i++) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template <typename T>
	FORCEINLINE constexpr void moveBackward(T* dst, T* src, const ssize start, const ssize end) noexcept {
		for (ssize i = start; i >= end; i--) {
			dst[i] = T(std::move(src[i]));
			src[i].~T();
		}
	}

	template <typename T>
	FORCEINLINE constexpr void moveConstruct(T* dst, T* src, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) { new(&dst[i]) T(std::move(src[i])); }
	}

	template <typename T>
	FORCEINLINE constexpr void moveConstructForward(T* dst, T* src, const ssize start, const ssize end) noexcept {
		for (ssize i = start; i < end; i++) { new(&dst[i]) T(std::move(src[i])); }
	}

	template <typename T>
	FORCEINLINE constexpr void moveConstructBackward(T* dst, T* src, const ssize start, const ssize end) noexcept {
		for (ssize i = start; i >= end; i--) { new(&dst[i]) T(std::move(src[i])); }
	}

	template <typename T>
	FORCEINLINE constexpr void memSet(T* dst, const T& src, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) { dst[i] = T(src); }
	}

	template <typename T>
	FORCEINLINE constexpr bool memCmp(const T* s1, const T* s2, const usize count = 1) {
		for (usize i = 0; i < count; i++) {
			if (s1[i] != s2[i])
				return false;
		}

		return true;
	}

	template <typename T>
	FORCEINLINE constexpr void destroy(T* ptr, const usize count = 1) noexcept {
		for (usize i = 0; i < count; i++) { ptr[i].~T(); }
	}

	template <typename T>
	FORCEINLINE constexpr void free(T* ptr, usize count = 1) noexcept {
		Utils::destroy(ptr, count);
		Utils::freeMemory(ptr);
	}
}
