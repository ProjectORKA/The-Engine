
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include <type_traits>
#include <cstring>
#include <utility>
#include <new>
#include <algorithm>
#include "Velox_Common.hpp"
#include "Velox_UtilityConcepts.hpp"

namespace TRE::Utils
{
    template<typename integral>
    FORCEINLINE constexpr bool IsAligned(integral x, usize a) noexcept
    {
        return (x & (integral(a) - 1)) == 0;
    }

    template<typename integral>
    FORCEINLINE constexpr integral AlignUp(integral x, usize a) noexcept
    {
        return integral((x + (integral(a) - 1)) & ~integral(a - 1));
    }

    template<typename integral>
    FORCEINLINE constexpr integral AlignDown(integral x, usize a) noexcept
    {
        return integral(x & ~integral(a - 1));
    }

    FORCEINLINE void* AllocateBytes(usize sz)
    {
        return ::operator new(sz);
    }

    FORCEINLINE void* AllocateBytes(usize sz, usize al)
    {
        return ::operator new(sz, static_cast<std::align_val_t>(al));
    }

    template<typename T>
    FORCEINLINE T* Allocate(usize sz)
    {
        // return static_cast<T*>(::operator new(sz * sizeof(T), static_cast<std::align_val_t>(alignof(T))));
        return static_cast<T*>(::operator new(sz * sizeof(T)));
    }

    FORCEINLINE void FreeMemory(void* ptr) noexcept
    {
        if (ptr)
            ::operator delete(ptr);
        //delete ptr;
    }

#if defined(COMPILER_MSVC) || __cplusplus >= 202002L
    // POD TYPES:
    template<POD T>
    FORCEINLINE constexpr void Copy(T* dst, const T* src, usize count = 1) noexcept
    {
        std::memcpy(dst, src, count * sizeof(T));
    }

    template<POD T>
    FORCEINLINE constexpr void CopyConstruct(T* dst, const T* src, usize count = 1) noexcept
    {
        Utils::Copy(dst, src, count);
    }

    template<POD T>
    FORCEINLINE constexpr void Move(T* dst, T* src, usize count = 1) noexcept
    {
        Utils::Copy(dst, src, count);
    }

    template<POD T>
    FORCEINLINE constexpr void MoveForward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        Utils::Copy(dst + start, src + end, end - start);
    }

    template<POD T>
    FORCEINLINE constexpr void MoveBackward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        std::memmove(dst + end, src + end, (start - end + 1) * sizeof(T));
    }

    template<POD T>
    FORCEINLINE constexpr void MoveConstruct(T* dst, T* src, usize count = 1) noexcept
    {
        Utils::Copy(dst, src, count);
    }

    template<POD T>
    FORCEINLINE constexpr void MoveConstructForward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        Utils::Copy(dst + start, src + start, end - start);
    }

    template<POD T>
    FORCEINLINE constexpr void MoveConstructBackward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        Utils::MoveBackward(dst, src, start, end);
    }

    template<POD T>
    FORCEINLINE constexpr void MemSet(T* dst, const T& src, usize count = 1) noexcept
    {
        std::fill(dst, dst + count, src);
    }

    template<POD T>
    FORCEINLINE constexpr bool MemCmp(const T* s1, const T* s2, usize count = 1)
    {
        return std::memcmp(s1, s2, sizeof(T) * count) == 0;
    }

    template<POD T>
    FORCEINLINE constexpr void Destroy(T* /*ptr*/, usize /*count = 1*/) noexcept
    {
    }

    template<POD T>
    FORCEINLINE constexpr void Free(T* ptr, usize /*count = 1*/) noexcept
    {
        Utils::FreeMemory(ptr);
    }
#endif

    // NON POD TYPES :
    template<typename T>
    FORCEINLINE constexpr void Copy(T* dst, const T* src, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            dst[i] = T(src[i]);
        }
    }

    template<typename T>
    FORCEINLINE constexpr void CopyConstruct(T* dst, const T* src, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            new (&dst[i]) T(src[i]);
        }
    }

    template<typename T>
    FORCEINLINE constexpr void Move(T* dst, T* src, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            dst[i] = T(std::move(src[i]));
            src[i].~T();
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MoveForward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        for (ssize i = start; i < end; i++) {
            dst[i] = T(std::move(src[i]));
            src[i].~T();
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MoveBackward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        for (ssize i = start; i >= end; i--) {
            dst[i] = T(std::move(src[i]));
            src[i].~T();
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MoveConstruct(T* dst, T* src, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            new (&dst[i]) T(std::move(src[i]));
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MoveConstructForward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        for (ssize i = start; i < end; i++) {
            new (&dst[i]) T(std::move(src[i]));
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MoveConstructBackward(T* dst, T* src, ssize start, ssize end) noexcept
    {
        for (ssize i = start; i >= end; i--) {
            new (&dst[i]) T(std::move(src[i]));
        }
    }

    template<typename T>
    FORCEINLINE constexpr void MemSet(T* dst, const T& src, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            dst[i] = T(src);
        }
    }

    template<typename T>
    FORCEINLINE constexpr bool MemCmp(const T* s1, const T* s2, usize count = 1)
    {
        for (usize i = 0; i < count; i++) {
            if (s1[i] != s2[i])
                return false;
        }

        return true;
    }

    template<typename T>
    FORCEINLINE constexpr void Destroy(T* ptr, usize count = 1) noexcept
    {
        for (usize i = 0; i < count; i++) {
            ptr[i].~T();
        }
    }

    template<typename T>
    FORCEINLINE constexpr void Free(T* ptr, usize count = 1) noexcept
    {
        Utils::Destroy(ptr, count);
        Utils::FreeMemory(ptr);
    }
}
