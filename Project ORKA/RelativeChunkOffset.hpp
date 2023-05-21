#pragma once

#include "OctreeId.hpp"
#include "PlanetCamera.hpp"

Vec3 relativeCameraPosition(const OctreeId& id, const ULLVec3& chunkLocation, Vec3 location);
