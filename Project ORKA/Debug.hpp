#pragma once

#include "Basics.hpp"

#define DEBUG

void logEvent(String message);
void logDebug(String message);
void logError(String message);



//functions
void beep();
void pause();
void logDebug(ULL& t);
void logDebug(Vec2& t);
void logDebug(Vec3 & t);
void logDebug(ULLVec3& t);
void logDebug(glm::highp_dvec3& t);
void logDebug(Matrix & t);

////templates
//template<typename T>
//void debugPrint(T t) {
//#ifdef DEBUG
//	std::cout << t << "\n";
//#endif // DEBUG
//}