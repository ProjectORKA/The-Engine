#pragma once

#include "Basics.hpp"

#define DEBUG

//#define MEMORY_LEAK_DETECTION //slows down the program a lot

#ifdef MEMORY_LEAK_DETECTION
#include <crtdbg.h>
#endif // MEMORY_LEAK_DETECTION


void logEvent(String message);
void logDebug(String message);
void logWarning(String message);
void logError(String message);

//functions
void beep();
void pause();
//screw templates lol
void logDebug(Int t);
void logDebug(UInt t);
void logDebug(ULL t);
void logDebug(Vec2 t);
void logDebug(Vec3 t);
void logDebug(Short t);
void logDebug(IVec2 t);
void logDebug(IVec3 t);
void logDebug(Matrix t);
void logDebug(ULLVec2 t);
void logDebug(ULLVec3 t);
void logDebug(glm::highp_dvec3 t);

template <typename T>
void logDebug(T t) {
#ifdef DEBUG
	std::cout << t << "\n";
#endif // DEBUG
}

struct DebugSystem {
	String log = "";
	UInt level = 0;

	void flush() {
		std::cout << log;
		log.clear();
		level = 0;
	}
};

extern Map<std::thread::id, DebugSystem> debugSystems;

struct DebugTracker { //doesnt work
	DebugTracker(String functionName);
	~DebugTracker();
};

void printDebugLog();