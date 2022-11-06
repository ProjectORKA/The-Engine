
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_AllocatorMisc.hpp"

TRE_NS_START

template<usize SIZE = 4096>
class LocalAllocator
{
public:
    using Traits = AllocTraits<true, SIZE>;

public:
    constexpr FORCEINLINE void* AllocateBytes(usize sz, usize al = 1)
    {
        // FIXME : this doesn't take into account alignement
        usize remaining = SIZE - m_Reserved;

        if (sz < remaining) {
            m_Reserved += sz;
            m_LatestSize = sz;
            return m_Buffer;
        }

        return Utils::AllocateBytes(sz, al);
    }

    template<typename T>
    constexpr FORCEINLINE T* Allocate(usize count)
    {
        void* data = this->AllocateBytes(sizeof(T) * count);
        return static_cast<T*>(data);
    }

    constexpr FORCEINLINE void* ReallocateBytes(void* ptr, usize sz, usize al = 1)
    {
        uint8* oldPtr = m_Buffer - m_LatestSize;
        usize remaining = SIZE - m_Reserved;
        usize addition = sz - m_LatestSize;

        if (oldPtr == ptr && addition < remaining) {
            m_LatestSize = sz;
            return ptr;
        }

        return Utils::AllocateBytes(sz);
    }

    template<typename T>
    constexpr FORCEINLINE T* Reallocate(T* ptr, usize count)
    {
        void* data = this->ReallocateBytes(ptr, sizeof(T) * count);
        return static_cast<T*>(data);
    }

    constexpr FORCEINLINE void FreeMemory(void* ptr) noexcept
    {
        if (ptr < m_Buffer && ptr >= m_Buffer + SIZE)
            Utils::FreeMemory(ptr);
    }

    constexpr LocalAllocator() = default;
    constexpr LocalAllocator(LocalAllocator&& other) = delete;
    constexpr LocalAllocator& operator=(LocalAllocator&& other) = delete;
    constexpr friend void Swap(LocalAllocator& first, LocalAllocator& second) noexcept = delete;

private:
    alignas(16) uint8 m_Buffer[SIZE];
    usize m_Reserved = 0;
    usize m_LatestSize = 0;
};

TRE_NS_END
