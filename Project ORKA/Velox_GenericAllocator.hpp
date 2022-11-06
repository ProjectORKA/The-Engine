
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_AllocatorMisc.hpp"

TRE_NS_START

class GenericAllocator
{
public:
    using Traits = AllocTraits<true, 0>;

public:
    FORCEINLINE void* AllocateBytes(usize sz, usize al = 1)
    {
        // FIXME : this is a temp hack
        if (al == 1)
            return Utils::AllocateBytes(sz);
        return Utils::AllocateBytes(sz, al);
    }

    template<typename T>
    FORCEINLINE T* Allocate(usize count)
    {
        void* data = this->AllocateBytes(sizeof(T) * count);
        return static_cast<T*>(data);
    }

    FORCEINLINE void* ReallocateBytes(void* ptr, usize sz, usize al = 1)
    {
        return this->AllocateBytes(sz, al);
    }

    template<typename T>
    FORCEINLINE T* Reallocate(T* ptr, usize count)
    {
        void* data = this->AllocateBytes(sizeof(T) * count);
        return static_cast<T*>(data);
    }

    FORCEINLINE void FreeMemory(void* ptr) noexcept
    {
        return Utils::FreeMemory(ptr);
    }

    constexpr FORCEINLINE friend void Swap(GenericAllocator& /*first*/, GenericAllocator& /*second*/) noexcept
    {

    }
private:

};

TRE_NS_END
