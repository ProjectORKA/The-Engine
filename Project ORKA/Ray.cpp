#include "Ray.hpp"

void Ray::advance(const Double distance) { origin = origin + ULLVec3(DVec3(distance) * DVec3(direction)); }
