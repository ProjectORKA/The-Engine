#pragma once

#include "OctreeID.hpp"
#include "PlanetCamera.hpp"

Vec3 relativeCameraPosition(const OctreeID& id, const ULLVec3& chunkLocation, Vec3 location);
