#pragma once

#include "Basics.hpp"

#define DEBUG

void logEvent(String message);
void logDebug(String message);
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
void logDebug(ULLVec3 t);
void logDebug(glm::highp_dvec3 t);
void logDebug(Matrix t);

template <typename T>
void logDebug(T t) {
	std::cout << t << "\n";
}