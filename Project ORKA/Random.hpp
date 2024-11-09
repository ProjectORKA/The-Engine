#pragma once
#include "Basics.hpp"

// there are two types of randomness ORKA provides
// fast and slow
// FAST: is really fast, but will repeat if the sequence is started over
// SLOW: is based on seed and time, which makes it unpredictable, but at the cost of performance
void randomizeSeed();

// fast variants
ULL    randomULLFast();
Vec3   randomVec3Fast();
Vec2   randomVec2Fast();
UInt   randomUIntFast();
Color  randomColorFast();
Float  randomFloatFast();
DVec2  randomDVec2Fast();
DVec3  randomDVec3Fast();
DVec3  randomDVec4Fast();
Double randomDoubleFast();
Vec3   randomUnitVec3Fast();
Vec2   randomUnitVec2Fast();
DVec2  randomUnitDVec2Fast();
DVec3  randomUnitDVec3Fast();
Int    randomIntFast(Int max);
ULL    randomULLFast(ULL max);
Int    randomIntFast(UInt max);
Int    randomIntFast(SizeT max);
UInt   randomUIntFast(UInt max);
Vec3   randomVec3Fast(Vec3 max);
Vec2   randomVec2Fast(Float max);
Vec3   randomVec3Fast(Float max);
Float  randomFloatFast(Float max);
Int    randomIntFast(Int min, Int max);
Vec3   randomVec3Fast(Vec3 min, Vec3 max);
Vec2   randomVec2Fast(Float min, Float max);
Vec4   randomVec4Fast(Float min, Float max);
Vec3   randomVec3Fast(Float min, Float max);
Vec2   randomPointInCircleFast(Float radius);
Vec3   randomPointInSphereFast(Float radius);
Vec3   randomPointOnSphereFast(Float radius);
Float  randomFloatFast(Double min, Double max);
Double randomDoubleFast(Double min, Double max);
Vec3   randomPointOnSphereAtPositionFast(Vec4 sphere);
Vec3   randomPointOnSphereAtPositionFast(Float radius, Vec3 position);
Vec3   randomVec3Fast(Float xMin, Float xMax, Float yMin, Float yMax, Float zMin, Float zMax);

template<typename T>
Float randomFloatFast(T min, T max)
{
	return randomFloatFast(toDouble(min), toDouble(max));
}

// slow variants
ULL    randomULLSlow();
Int    randomIntSlow();
Vec3   randomVec3Slow();
Color  randomColorSlow();
Float  randomFloatSlow();
Int    randomIntSlow(Int max);
ULL    randomULLSlow(ULL max);
Int    randomIntSlow(SizeT max);
ULL    randomULLSlow(ULL min, ULL max);
Vec2   randomVec2Slow(Float min, Float max);
Vec3   randomVec3Slow(Float min, Float max);
Float  randomFloatSlow(Double min, Double max);
Double randomDoubleSlow(Double min, Double max);

template <typename T>
Float randomFloatSlow(T min, T max)
{
	return randomFloatSlow(toDouble(min), toDouble(max));
}

using Token = ULL;

Token generateRandomToken();
