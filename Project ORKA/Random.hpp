#pragma once
#include "Basics.hpp"

// there are two types of randomness ORKA provides
// fast and slow
// FAST: is really fast, but will repeat if the sequence is started over
// SLOW: is based on seed and time, which makes it unpredictable, but at the cost of performance
void randomizeSeed();

// fast variants

UInt   randomUIntFast();
UInt   randomUIntFast(UInt max);

ULL    randomULLFast();

Int    randomIntFast(Int max);
Int    randomIntFast(ULL max);
Int    randomIntFast(UInt max);
Int    randomIntFast(Int min, Int max);

Float  randomFloatFast();
Float  randomFloatFast(Float max);
Float  randomFloatFast(Float min, Float max);
Float  randomFloatFast(Double min, Double max);

Double randomDoubleFast();
Double randomDoubleFast(Double min, Double max);

Vec2   randomVec2Fast();
Vec2   randomUnitVec2Fast();
Vec2   randomVec2Fast(Float max);
Vec2   randomVec2Fast(Float min, Float max);
Vec2   randomPointInCircleFast(Float radius);

Vec3   randomVec3Fast();
Vec3   randomUnitVec3Fast();
Vec3   randomVec3Fast(Vec3 max);
Vec3   randomVec3Fast(Float max);
Vec3   randomVec3Fast(Vec3 min, Vec3 max);
Vec3   randomVec3Fast(Float min, Float max);
Vec3   randomPointInSphereFast(Float radius);
Vec3   randomPointOnSphereFast(Float radius);
Vec3   randomPointOnSphereAtLocationFast(Vec4 sphere);
Vec3   randomPointOnSphereAtLocationFast(Float radius, Vec3 location);
Vec3   randomVec3Fast(Float xMin, Float xMax, Float yMin, Float yMax, Float zMin, Float zMax);

Vec4   randomVec4Fast(Float min, Float max);

DVec2   randomUnitDVec2Fast();
DVec2  randomDVec2Fast();

DVec3  randomDVec3Fast();
DVec3  randomDVec4Fast();
DVec3 randomUnitDVec3Fast();


// slow variants
ULL    randomULLSlow();
Int    randomIntSlow();
Vec3   randomVec3Slow();
Float  randomFloatSlow();
Int    randomIntSlow(Int max);
ULL    randomULLSlow(ULL max);
Int    randomIntSlow(SizeT max);
ULL    randomULLSlow(ULL min, ULL max);
Float  randomFloatSlow(Float min, Float max);
Float  randomFloatSlow(Double min, Double max);
Double randomDoubleSlow(Double min, Double max);
