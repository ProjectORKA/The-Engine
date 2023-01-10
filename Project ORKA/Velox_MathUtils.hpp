
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include "Velox_CompilerIntrin.hpp"

namespace Math
{
    FORCEINLINE static uint32 NextPow2(uint32 v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    FORCEINLINE static uint64 NextPow2(uint64 v)
    {
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v |= v >> 32;
        v++;
        return v;
    }

    /*FORCEINLINE static usize NextPow2(usize v)
    {
        if constexpr (sizeof(usize) == sizeof(uint32)){
            return NextPow2(static_cast<uint32>(v));
        }else{
            return NextPow2(static_cast<uint64>(v));
        }
    }*/

    FORCEINLINE static uint32 Log2OfPow2(uint32 value) {
        if (!value)
            return 0;
    #if defined(COMPILER_CLANG) || defined (COMPILER_GCC) || defined(COMPILER_MSVC)
        return sizeof(uint32_t) * CHAR_BIT - __builtin_clz(value) - 1;
    #else
        return log2(value);
    #endif
    }

    FORCEINLINE static uint64 Log2OfPow2(uint64 value)
    {
        if (!value)
            return 0;
#if defined(COMPILER_CLANG) || defined (COMPILER_GCC) || defined(COMPILER_MSVC)
        return sizeof(uint64) * CHAR_BIT - __builtin_clzll(value) - 1;
#else
        return log2(value);
#endif
    }

    /*FORCEINLINE static usize Log2OfPow2(usize v)
    {
        if constexpr (sizeof(usize) == sizeof(uint32)){
            return Log2OfPow2(static_cast<uint32>(v));
        }else{
            return Log2OfPow2(static_cast<uint64>(v));
        }
    }*/

};