
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Platform.hpp"
#include "Velox_PlatformInclude.hpp"

#ifdef COMPILER_MSVC
    static inline int __builtin_ctz(unsigned x) {
        unsigned long ret;
        _BitScanForward(&ret, x);
        return (int)ret;
    }

    static inline int __builtin_ctzll(unsigned long long x) {
        unsigned long ret;
        _BitScanForward64(&ret, x);
        return (int)ret;
    }

    static inline int __builtin_ctzl(unsigned long x) {
        return sizeof(x) == 8 ? __builtin_ctzll(x) : __builtin_ctz((uint32_t)x);
    }

    static inline int __builtin_clz(unsigned x) {
        //unsigned long ret;
        //_BitScanReverse(&ret, x);
        //return (int)(31 ^ ret);
        return (int)__lzcnt(x);
    }

    static inline int __builtin_clzll(unsigned long long x) {
        //unsigned long ret;
        //_BitScanReverse64(&ret, x);
        //return (int)(63 ^ ret);
        return (int)__lzcnt64(x);
    }

    static inline int __builtin_clzl(unsigned long x) {
        return sizeof(x) == 8 ? __builtin_clzll(x) : __builtin_clz((uint32_t)x);
    }

    #ifdef __cplusplus
        static inline int __builtin_ctzl(unsigned long long x) {
            return __builtin_ctzll(x);
        }

        static inline int __builtin_clzl(unsigned long long x) {
            return __builtin_clzll(x);
        }
    #endif
#endif
