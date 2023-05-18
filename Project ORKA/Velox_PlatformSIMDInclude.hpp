#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include "Platform.hpp"

// Include the appropriate header file for intrinsic functions
#if SIMD_SUPPORTED_LEVEL > 7                       // AVX2 and later
#if defined (COMPILER_GCC) && ! defined (COMPILER_INTEL)
	#include <x86intrin.h>                 // x86intrin.h includes header files for whatever instruction 
										   // sets are specified on the compiler command line, such as:
										   // xopintrin.h, fma4intrin.h
#else
		#include <immintrin.h>                 // MS version of immintrin.h covers AVX, AVX2 and FMA3
#endif // __GNUC__
#elif SIMD_SUPPORTED_LEVEL == 7
	#include <immintrin.h>                 // AVX
#elif SIMD_SUPPORTED_LEVEL == 6
	#include <nmmintrin.h>                 // SSE4.2
#elif SIMD_SUPPORTED_LEVEL == 5
	#include <smmintrin.h>                 // SSE4.1
#elif SIMD_SUPPORTED_LEVEL == 4
	#include <tmmintrin.h>                 // SSSE3
#elif SIMD_SUPPORTED_LEVEL == 3
	#include <pmmintrin.h>                 // SSE3
#elif SIMD_SUPPORTED_LEVEL == 2
#include <emmintrin.h>                 // SSE2
#elif SIMD_SUPPORTED_LEVEL == 1
	#include <xmmintrin.h>                 // SSE
#endif // SIMD_SUPPORTED_LEVEL

#if SIMD_SUPPORTED_LEVEL >= 8 && !defined(__FMA__)
// Assume that all processors that have AVX2 also have FMA3
#if defined(COMPILER_GCC) && ! defined(COMPILER_INTEL) && ! defined(COMPILER_CLANG)
	// Prevent error message in g++ when using FMA intrinsics with avx2:
	#pragma message "It is recommended to specify also option -mfma when using -mavx2 or higher"
#else
	#define __FMA__  1
#endif
#endif

// AMD  instruction sets
#if defined(__XOP__) || defined(__FMA4__)
#ifdef COMPILER_GCC
		#include <x86intrin.h>                 // AMD XOP (Gnu)
#elif COMPILER_MSVC
		#include <ammintrin.h>                 // AMD XOP (Microsoft)
#endif //  __GNUC__
#elif defined(__SSE4A__)					   // AMD SSE4A
	#include <ammintrin.h>
#endif // __XOP__ 

// FMA3 instruction set
#if defined(__FMA__) && (defined(COMPILER_GCC) || defined(COMPILER_CLANG))  && ! defined (COMPILER_INTEL)
	#include <fmaintrin.h> 
#endif // __FMA__ 

// FMA4 instruction set
#if defined (__FMA4__) && (defined(COMPILER_GCC) || defined(COMPILER_CLANG))
	#include <fma4intrin.h> // must have both x86intrin.h and fma4intrin.h, don't know why
#endif // __FMA4__

#define SHUFFLE_PARAM(x, y, z, w) \
	(uint8)((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))
#if SIMD_SUPPORTED_LEVEL >= 1
constexpr int32 ALIGN(16) __MASKSIGNs_[4] = {
	static_cast<int32>(0x80000000), static_cast<int32>(0x80000000), static_cast<int32>(0x80000000),
	static_cast<int32>(0x80000000)
};
constexpr int32 ALIGN(16) _Sign_PNPN[4] = {
	0x00000000, static_cast<int32>(0x80000000), 0x00000000, static_cast<int32>(0x80000000)
};
constexpr int32 ALIGN(16) _Sign_NPNP[4] = {
	static_cast<int32>(0x80000000), 0x00000000, static_cast<int32>(0x80000000), 0x00000000
};
constexpr int32 ALIGN(16) _Sign_PNNP[4] = {
	0x00000000, static_cast<int32>(0x80000000), static_cast<int32>(0x80000000), 0x00000000
};
const int32 ALIGN(16) __0FFF_[4] = {
	static_cast<int32>(0xFFFFFFFF), static_cast<int32>(0xFFFFFFFF), static_cast<int32>(0xFFFFFFFF), 0x00000000
};
constexpr float ALIGN(16) __ZERONE_[4] = {1.0f, 0.0f, 0.0f, 1.0f};
constexpr float ALIGN(16) __1000_[4] = {1.0f, 0.0f, 0.0f, 0.0f};

#define _MASKSIGNs_ (*(__m128*)&__MASKSIGNs_)      // - - - -
#define Sign_PNPN   (*(__m128*)&_Sign_PNPN)        // + - + -
#define Sign_NPNP   (*(__m128*)&_Sign_NPNP)        // - + - +
#define Sign_PNNP   (*(__m128*)&_Sign_PNNP)        // + - - +
#define _0FFF_      (*(__m128*)&__0FFF_)           // 0 * * *
#define _ZERONE_    (*(__m128*)&__ZERONE_)         // 1 0 0 1
#define _1000_      (*(__m128*)&__1000_)           // 1 0 0 0

#define _mm_replicate_x_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0, 0, 0, 0))
#define _mm_replicate_y_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1, 1, 1, 1))
#define _mm_replicate_z_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2, 2, 2, 2))
#define _mm_replicate_w_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3, 3, 3, 3))
#define _mm_madd_ps(a, b, c)  _mm_add_ps(_mm_mul_ps(a, b), c)

// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, SHUFFLE_PARAM(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, SHUFFLE_PARAM(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)
// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, SHUFFLE_PARAM(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)
#define _mm_ror_ps(vec,i)   (((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))) : (vec))
#define _mm_rol_ps(vec,i)   (((i)%4) ? (_mm_shuffle_ps(vec,vec, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))) : (vec))

// for row major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A1 |
//                                              | A2  A3 |
// 2x2 row major Matrix multiply A*B
static FORCEINLINE __m128 Mat2Mul(const __m128 vec1, const __m128 vec2) {
	return
		_mm_add_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 0, 3, 0, 3)),
		           _mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}

// 2x2 row major Matrix adjugate multiply (A#)*B
static FORCEINLINE __m128 Mat2AdjMul(const __m128 vec1, const __m128 vec2) {
	return
		_mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3, 3, 0, 0), vec2),
		           _mm_mul_ps(VecSwizzle(vec1, 1, 1, 2, 2), VecSwizzle(vec2, 2, 3, 0, 1)));
}

// 2x2 row major Matrix multiply adjugate A*(B#)
static FORCEINLINE __m128 Mat2MulAdj(const __m128 vec1, const __m128 vec2) {
	return
		_mm_sub_ps(_mm_mul_ps(vec1, VecSwizzle(vec2, 3, 0, 3, 0)),
		           _mm_mul_ps(VecSwizzle(vec1, 1, 0, 3, 2), VecSwizzle(vec2, 2, 1, 2, 1)));
}

// Use dummy = a recently-dead variable that vec depends on,
//  so it doesn't introduce a false dependency,
//  and the compiler probably still has it in a register
static FORCEINLINE __m128d _mm_highhalf_pd(__m128d dummy, __m128d vec) {
#if SIMD_SUPPORTED_LEVEL >= 7
			// With 3-operand AVX instructions, don't create an extra dependency on something we don't need anymore.
			(void)dummy;
			return _mm_unpackhi_pd(vec, vec);
#else
	// Without AVX, we can save a MOVAPS with MOVHLPS into a dead register
	const __m128 tmp = _mm_castpd_ps(dummy);
	const __m128d high = _mm_castps_pd(_mm_movehl_ps(tmp, _mm_castpd_ps(vec)));
	return high;
#endif
}

// Use dummy = a recently-dead variable that vec depends on,
//  so it doesn't introduce a false dependency,
//  and the compiler probably still has it in a register
static FORCEINLINE __m128 _mm_highhalf_ps(__m128 dummy, __m128 vec) {
#if SIMD_SUPPORTED_LEVEL >= 7
		// With 3-operand AVX instructions, don't create an extra dependency on something we don't need anymore.
		(void)dummy;
		return _mm_unpackhi_ps(vec, vec);
#else
	// Without AVX, we can save a MOVAPS with MOVHLPS into a dead register
	const __m128 tmp = dummy;
	const __m128 high = _mm_movehl_ps(tmp, vec);
	return high;
#endif
}

using _m128 = union _m128 {
	__m128 xmm;
	float m128_f32[4];
	uint64 m128_u64[2];
	int8 m128_i8[16];
	int16 m128_i16[8];
	int32 m128_i32[4];
	int64 m128_i64[2];
	uint8 m128_u8[16];
	uint16 m128_u16[8];
	uint32 m128_u32[4];
	_m128() {}
	FORCEINLINE _m128(const __m128& x) { xmm = x; }
};

using _m128i = union _m128i {
	__m128i xmm;
	int8 m128i_i8[16];
	int16 m128i_i16[8];
	int32 m128i_i32[4];
	int64 m128i_i64[2];
	uint8 m128i_u8[16];
	uint16 m128i_u16[8];
	uint32 m128i_u32[4];
	uint64 m128i_u64[2];
	_m128i() {}
	FORCEINLINE _m128i(const __m128i& x) { xmm = x; }
};

#endif
