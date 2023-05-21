#pragma once

#include <iostream>
#include "Basics.hpp"

#define DEBUG
//#define MEMORY_LEAK_DETECTION //slows down the program a lot

#ifdef MEMORY_LEAK_DETECTION
#include <crtdbg.h>
#endif // MEMORY_LEAK_DETECTION

template<typename T>
void logDebug(Vector<T> vec)
{
#ifdef DEBUG
	for(auto v : vec) logDebug(v);
#endif // DEBUG
};

constexpr bool showEvents = true;
constexpr bool showDebug = true;
constexpr bool showWarning = true;
constexpr bool showError = true;

inline void beep()
{
	std::cout << '\a';
}

inline void pause()
{
#ifdef DEBUG
#ifdef _WIN32
	system("pause");
#else
	std::cout << "Thread was paused..." << "\n";
	char a;
	std::cin >> a;
#endif
#endif // DEBUG
}

inline void logEvent(const String& message)
{
#ifdef DEBUG
	if(showEvents) std::cout << "Event: " << message << "\n";
#endif // DEBUG
}

inline void logDebug(const String& message)
{
#ifdef DEBUG
	std::cout << "Debug: " << message << "\n";
#endif // DEBUG
}

inline void logError(const String& message)
{
#ifdef DEBUG
	if(showError) std::cout << "Error: " << message << "\n";
	beep();
	__debugbreak();
	pause();
#endif // DEBUG
}

inline void logWarning(const String& message)
{
#ifdef DEBUG
	beep();
	if(showWarning) std::cout << "Warning: " << message << "\n";
#endif // DEBUG
}

inline void logDebug(Matrix v)
{
#ifdef DEBUG
	std::cout << "[ " << v[0][0] << "  " << v[0][1] << "  " << v[0][2] << "  " << v[0][3] << " ]\n";
	std::cout << "[ " << v[1][0] << "  " << v[1][1] << "  " << v[1][2] << "  " << v[1][3] << " ]\n";
	std::cout << "[ " << v[2][0] << "  " << v[2][1] << "  " << v[2][2] << "  " << v[2][3] << " ]\n";
	std::cout << "[ " << v[3][0] << "  " << v[3][1] << "  " << v[3][2] << "  " << v[3][3] << " ]\n";
#endif // DEBUG
}

inline void logDebug(const ULL v)
{
#ifdef DEBUG
	std::cout << "Debug: (" << std::bitset<64>(v) << ")" << v << "\n";
#endif // DEBUG
}

inline void logDebug(const Int v)
{
#ifdef DEBUG
	std::cout << v << "\n";
#endif // DEBUG
}

inline void logDebug(const UInt v)
{
#ifdef DEBUG
	std::cout << v << "\n";
#endif // DEBUG
}

inline void logDebug(const Vec2 v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << ")" << "\n";
#endif // DEBUG
};

inline void logDebug(const Vec3 v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << ")" << "\n";
#endif // DEBUG
}

inline void logDebug(const Vec4 v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << "|" << v.w << ")" << "\n";
#endif // DEBUG
}

inline void logDebug(const Short v)
{
#ifdef DEBUG
	std::cout << v << "\n";
#endif // DEBUG
}

inline void logDebug(const IVec3 v)
{
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << ")" << "\n";
}

inline void logDebug(const IVec2 v)
{
	std::cout << "(" << v.x << "|" << v.y << ")" << "\n";
}

inline void logDebug(const Path& v)
{
	logDebug(v.string());
}

inline void logDebug(const char* v)
{
#ifdef DEBUG
	std::cout << "Debug: " << v << "\n";
#endif // DEBUG
}

inline void logDebug(const DVec3& v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << ")" << "\n";
#endif // DEBUG
}

inline void logDebug(const ULLVec2 v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << ")" << "\n";
#endif // DEBUG
}

inline void logDebug(const ULLVec3& v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << ")" << "\n";
#endif // DEBUG
}

inline void logDebug(const ULLVec4& v)
{
#ifdef DEBUG
	std::cout << "(" << v.x << "|" << v.y << "|" << v.z << "|" << v.w << ")" << "\n";
#endif // DEBUG
}

inline void printLaunchProperties(const Int argc, Char* argv[])
{
	for(Int i = 0; i < argc; i++)
	{
		std::cout << "Launch properties:" << argv[i] << "\n";
	}
}