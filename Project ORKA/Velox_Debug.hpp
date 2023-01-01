
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <stdio.h>
#include <ctime>
#include "Platform.hpp"

#if defined(COMPILER_MSVC)
	#pragma warning(disable:4996) // DISABLE VS WARNING
#endif

#if !defined(_DEBUG) || defined(NDEBUG)
    #define TRE_ASSERTF(condition, ...) {};
    #define TRE_ASSERT(condition) {};
    #define ASSERTF(condition, ...) {};
    #define ASSERT(condition) {};
    #undef DEBUG
#else
    #include "Velox_Debug.hpp"
    #include "Velox_Logging.hpp"

#if !defined(DEBUG)
    #define DEBUG
#endif

	#if defined(OS_WINDOWS) && (CPU_ARCH == CPU_ARCH_x86_64 || CPU_ARCH == CPU_ARCH_x86)
        //#include <intrin.h>
		#define DEBUG_BREAK() __debugbreak() 
	#elif defined(OS_UNIX) || defined(OS_LINUX) && !defined(COMPILER_GCC)
		#include <signal.h>
		#define DEBUG_BREAK() raise(SIGINT);
    #elif defined(OS_UNIX) || defined(OS_LINUX) || defined(OS_APPLE)
        #define DEBUG_BREAK() __builtin_trap();
	#else
		#define DEBUG_BREAK()
	#endif
	
	#if defined(OS_WINDOWS)
		#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#else
		#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	#endif

    #if defined(TRE_ASSERTF)
        #undef TRE_ASSERTF
	#endif
    #if defined(TRE_ASSERT)
        #undef TRE_ASSERT
    #endif

    #define TRE_ASSERTF(condition, ...) \
        if (!(bool)(condition)){ \
            char assert_msg[512]; \
            snprintf(assert_msg, 512, __VA_ARGS__); \
            TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] %s.", __FILENAME__, __LINE__, __FUNCTION__, assert_msg); \
            DEBUG_BREAK(); \
        }\

    #define TRE_ASSERT(condition) \
        if (!(bool)(condition)){ \
            TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] Assert occured (Condition: %s).", __FILENAME__, __LINE__, __FUNCTION__, #condition); \
            DEBUG_BREAK(); \
        } \

    #define ASSERTF(condition, ...) \
        if (!(bool)(condition)){ \
            char assert_msg[512]; \
            snprintf(assert_msg, 512, ##__VA_ARGS__); \
            TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] %s.", __FILENAME__, __LINE__, __FUNCTION__, assert_msg); \
            DEBUG_BREAK(); \
        }\

    #define ASSERT(condition) \
        if (!(bool)(condition)){ \
            TRE::Log::Write(TRE::Log::ASSERT, "[%s::%d::%s] Assert occured (Condition: %s).", __FILENAME__, __LINE__, __FUNCTION__, #condition); \
            DEBUG_BREAK(); \
        } \

#endif
