
#pragma once

//written by Omarito
//	modified by Blendurian

#pragma comment(lib,"user32.lib") 

#define NOMINMAX
#include <windows.h>

//Defines for SIMD-enabled CPU architectures, and supported
//feature levels
#define CPU_ARCH_OTHER  0
#define CPU_ARCH_x86    1
#define CPU_ARCH_x86_64 2

#define SIMD_LEVEL_NONE       0
#define SIMD_LEVEL_x86_SSE    1
#define SIMD_LEVEL_x86_SSE2   2
#define SIMD_LEVEL_x86_SSE3   3
#define SIMD_LEVEL_x86_SSSE3  4
#define SIMD_LEVEL_x86_SSE4_1 5
#define SIMD_LEVEL_x86_SSE4_2 6
#define SIMD_LEVEL_x86_AVX    7
#define SIMD_LEVEL_x86_AVX2   8
#define SIMD_LEVEL_x86_AVX512 9

//Detect CPU architecture
#if (defined(_M_AMD64) || defined(_M_X64) || defined(__amd64) ) || defined(__x86_64__) || defined(__amd64__) || defined(__x86_64) // AMD64 / x86
	#define SIMD_CPU_ARCH CPU_ARCH_x86_64
	#define CPU_ARCH CPU_ARCH_x86_64
#elif defined(_M_X86) || defined(__i386__) || defined(_X86_) || defined(_M_IX86) // Intel x86
	#define SIMD_CPU_ARCH CPU_ARCH_x86
	#define CPU_ARCH CPU_ARCH_x86
#else
	#define SIMD_CPU_ARCH CPU_ARCH_OTHER
	#define CPU_ARCH CPU_ARCH_OTHER
#endif

//Detect supported SIMD features
#if defined(CPU_ARCH_x86) || defined(CPU_ARCH_x86_64)
	#if defined(INSTRSET)
		#define SIMD_SUPPORTED_LEVEL INSTRSET
	#elif defined(__AVX2__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_AVX2
	#elif defined(__AVX__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_AVX
	#elif defined(__SSE4_2__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE4_2
	#elif defined(__SSE4_1__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE4_1
	#elif defined(__SSSE3__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSSE3
	#elif defined(__SSE3__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE3
	#elif defined(__SSE2__) || defined(CPU_ARCH_x86_64)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE2
	#elif defined(__SSE__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_SSE
	#elif defined(__AVX512F__) || defined(__AVX512__)
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_x86_AVX512
	#elif defined(_M_IX86_FP)
		#define SIMD_SUPPORTED_LEVEL _M_IX86_FP
	#else
		#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_NONE
	#endif
#else
	#define SIMD_SUPPORTED_LEVEL SIMD_LEVEL_NONE
#endif

// Detect operating system
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
    #define OS_WINDOWS
#elif defined(__linux__) || defined(__linux)
	#define OS_LINUX
#elif defined(__unix)
	#define OS_UNIX
#elif defined(__ANDROID__)
	#define OS_ANDROID
#elif defined(__APPLE__)
	#define OS_APPLE
#else
	#define OPERATING_SYSTEM_OTHER
#endif

// Detect compiler
#if defined(__clang__)
	#define COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
	#define COMPILER_GCC
#elif defined(_MSC_VER)
	#define COMPILER_MSVC
#elif defined(__INTEL_COMPILER)
	#define COMPILER_INTEL
#else
	#define COMPILER_OTHER
#endif


#if defined(COMPILER_MSVC)
#	define ALIGN(X) __declspec(align(X))
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#   define ALIGN(X) __attribute__ ((aligned(X)))
#endif
