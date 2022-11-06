
//written by Omarito
//modified by
//  Blendurian

#pragma once

#include "Velox_Common.hpp"

TRE_NS_START

template<bool R = false, usize CAP = 0>
struct AllocTraits
{
    constexpr static bool HAVE_REALLOC = R;
    constexpr static usize STATIC_CAP = CAP;
    constexpr static bool IS_STATIC = STATIC_CAP != 0;
};

template<typename T>
concept AllocConcept = requires(T t, usize sz, usize al, void* ptr) {
    typename T::Traits;
    { t.AllocateBytes(sz, al) };
    { t.FreeMemory(ptr) };
};

TRE_NS_END
