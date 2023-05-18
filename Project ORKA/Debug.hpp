#pragma once

#include <iostream>
#include "Basics.hpp"

#define DEBUG
//#define MEMORY_LEAK_DETECTION //slows down the program a lot

#ifdef MEMORY_LEAK_DETECTION
#include <crtdbg.h>
#endif // MEMORY_LEAK_DETECTION


void logEvent(const String& message);
void logDebug(const String& message);
void logWarning(const String& message);
void logError(const String& message);

//functions
void beep();
void pause();
//screw templates lol
void logDebug(Int t);
void logDebug(UInt t);
void logDebug(ULL t);
void logDebug(Vec2 t);
void logDebug(Vec3 t);
void logDebug(const Path& t);
void logDebug(Vec4 t);
void logDebug(Short t);
void logDebug(IVec2 t);
void logDebug(IVec3 t);
void logDebug(Matrix t);
void logDebug(ULLVec2 t);
void logDebug(const ULLVec3& t);
void logDebug(const char* t);
void logDebug(const glm::highp_dvec3& t);
void printLaunchProperties(Int argc, Char* argv[]);

//template <typename T>
//void logDebug(T t) {
//#ifdef DEBUG
//	std::cout << "Debug: " << t << "\n";
//#endif // DEBUG
//}

template <typename T>
void logDebug(Vector<T> vec) {
#ifdef DEBUG
	for (auto v : vec) logDebug(v);
#endif // DEBUG
};
