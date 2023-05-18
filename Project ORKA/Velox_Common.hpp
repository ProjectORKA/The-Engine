#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <limits.h>
#include <inttypes.h>
#include "Platform.hpp"

//Include known-size integer files, based on compiler. Some compilers do not have these
//files, so they must be created manually.
#if defined(__GNUC__) || defined(__clang__) || (defined(_MSC_VER) && _MSC_VER >= 1600)
// Compilers supporting C99 or C++0x have stdint.h defining these integer types	
#include <stdint.h>
#include <cstddef>
#elif defined(_MSC_VER)
	typedef signed   __int8  int8_t;
	typedef unsigned __int8  uint8_t;
	typedef signed   __int16 int16_t;
	typedef unsigned __int16 uint16_t;
	typedef signed   __int32 int32_t;
	typedef unsigned __int32 uint32_t;
	typedef signed   __int64 int64_t;
	typedef unsigned __int64 uint64_t;
	typedef uint64_t uintptr_t;
	typedef int64_t intptr_t;
	typedef int16_t wchar_t;

#ifndef _INTPTR_T_DEFINED
		#define _INTPTR_T_DEFINED
#ifdef  __x86_64__
			typedef int64_t intptr_t;
#else
			typedef int32_t intptr_t;
#endif
#endif
#else
	// sizeof(char) == 1
	// sizeof(char) <= sizeof(short)
	// sizeof(short) <= sizeof(int)
	// sizeof(int) <= sizeof(long)
	// sizeof(long) <= sizeof(long long)
	// sizeof(char) * CHAR_BIT >= 8
	// sizeof(short) * CHAR_BIT >= 16
	// sizeof(int) * CHAR_BIT >= 16
	// sizeof(long) * CHAR_BIT >= 32
	// sizeof(long long) * CHAR_BIT >= 64

	typedef signed   char      int8_t;
	typedef unsigned char      uint8_t;
	typedef signed   short int int16_t;
	typedef unsigned short int uint16_t;
	typedef signed   int       int32_t;
	typedef unsigned int       uint32_t;
	typedef long long          int64_t;
	typedef unsigned long long uint64_t;
	typedef uint64_t uintptr_t;
	typedef int64_t intptr_t;
	typedef int16_t wchar_t;
#ifdef  __x86_64__
		typedef int64_t intptr_t;
#else
		typedef int32_t intptr_t;
#endif
#endif

/*
 * Types that differ between LLP64 and LP64 architectures - in LLP64,
 * pointers are 64 bits, but 'long' is still 32 bits. Win64 appears
 * to be the only LLP64 architecture in current use.
*/
#ifdef _WIN64
using ssize_type = signed long long;
using usize_type = unsigned long long;
#elif _WIN32
	typedef signed   int     	ssize_type;
	typedef unsigned int     	usize_type;
#else
	typedef signed	 long long  ssize_type;
	typedef std::size_t 		usize_type;
#endif

#if defined(OS_WINDOWS)
using ssize_t = ssize_type;
using usize_t = usize_type;
#endif

#if CPU_ARCH == CPU_ARCH_x86_64
#define SZi PRIi64
#define SZu PRIu64
#elif CPU_ARCH == CPU_ARCH_x86
	#define SZi PRIi32 
	#define SZu PRIu32
#endif

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint = uint32_t;
using uint64 = uint64_t;
using ssize = ssize_type;
using usize = usize_type;
using intptr = intptr_t;
using uintptr = uintptr_t;

#define INLINE inline

#ifdef COMPILER_MSVC
#define FORCEINLINE __forceinline
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
	#define FORCEINLINE inline __attribute__ ((always_inline))
#else
	#define FORCEINLINE inline
#endif

#if __cplusplus < 199711L
	#define nullptr NULL
	#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

#define NULL_COPY_AND_ASSIGN(T) \
	T(const T& other) {(void)other;} \
	void operator=(const T& other) { (void)other; }

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a)	 ((a < 0) ? -a : a)

#define BITS_PER_BYTE CHAR_BIT


/******************* SUPRESS WARNINGS *******************/

#if defined(COMPILER_MSVC)
#pragma warning(disable : 4201) 							// Remove namesless struct/union warnning
//#pragma warning(disable : 4201) 							// Remove multichar warnning
#elif defined(COMPILER_GCC)
	#pragma GCC diagnostic ignored "-Wpedantic"					// Remove namesless struct/union warnning
	#pragma GCC diagnostic ignored "-Wunused-function"			// Remove namesless struct/union warnning
#elif defined(COMPILER_CLANG)
	#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"	// Remove namesless struct/union warnning
	#pragma clang diagnostic ignored "-Wnested-anon-types"		// Remove namesless struct/union warnning
#endif

/******************* BIT ENDIANNESS *******************/

#if defined(OS_WINDOWS)

#define LITTLE_ENDIAN 0x41424344UL
#define BIG_ENDIAN    0x44434241UL
#define PDP_ENDIAN    0x42414443UL
#define ENDIAN_ORDER  ('ABCD')
#define ENDIANNESS	  0

#if ENDIAN_ORDER == LITTLE_ENDIAN
#undef ENDIANNESS
#define ENDIANNESS LITTLE_ENDIAN
#elif ENDIAN_ORDER == BIG_ENDIAN
		#undef ENDIANNESS
		#define ENDIANNESS BIG_ENDIAN
#elif ENDIAN_ORDER == PDP_ENDIAN
		#undef ENDIANNESS
		#define ENDIANNESS PDP_ENDIAN
#else
		#error "Can't determine the endianness."
#endif

#elif defined(OS_LINUX) || defined(OS_UNIX) || defined(OS_APPLE) || defined(OS_ANDROID)

	#include <sys/types.h>
	
	#define ENDIANNESS	  0

#if __BYTE_ORDER == __BIG_ENDIAN
		#undef ENDIANNESS
		#define ENDIANNESS BIG_ENDIAN
#elif __BYTE_ORDER == __LITTLE_ENDIAN
		#undef ENDIANNESS
		#define ENDIANNESS LITTLE_ENDIAN
#else
		#error "Can't determine the endianness."
#endif
#endif

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#define TRE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#define TRE_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define TRE_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#define TRE_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#define TRE_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define TRE_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#define TRE_FUNC_SIG __func__
#else
#define TRE_FUNC_SIG "TRE_FUNC_SIG unknown!"
#endif
