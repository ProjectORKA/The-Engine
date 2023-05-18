#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Platform.hpp"
#include "Velox_PlatformInclude.hpp"

#ifdef COMPILER_MSVC
static inline int __builtin_ctz(const unsigned x) {
	unsigned long ret;
	_BitScanForward(&ret, x);
	return static_cast<int>(ret);
}

static inline int __builtin_ctzll(const unsigned long long x) {
	unsigned long ret;
	_BitScanForward64(&ret, x);
	return static_cast<int>(ret);
}

static inline int __builtin_ctzl(const unsigned long x) {
	return sizeof(x) == 8 ? __builtin_ctzll(x) : __builtin_ctz(static_cast<uint32_t>(x));
}

static inline int __builtin_clz(const unsigned x) {
	//unsigned long ret;
	//_BitScanReverse(&ret, x);
	//return (int)(31 ^ ret);
	return static_cast<int>(__lzcnt(x));
}

static inline int __builtin_clzll(const unsigned long long x) {
	//unsigned long ret;
	//_BitScanReverse64(&ret, x);
	//return (int)(63 ^ ret);
	return static_cast<int>(__lzcnt64(x));
}

static inline int __builtin_clzl(const unsigned long x) {
	return sizeof(x) == 8 ? __builtin_clzll(x) : __builtin_clz(static_cast<uint32_t>(x));
}

#ifdef __cplusplus
static inline int __builtin_ctzl(const unsigned long long x) { return __builtin_ctzll(x); }

static inline int __builtin_clzl(const unsigned long long x) { return __builtin_clzll(x); }
#endif
#endif
