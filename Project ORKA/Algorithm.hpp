
#pragma once

#include "Basics.hpp"

Vec3 vectorFromAToB(Vec3 a, Vec3 b);
void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius);
void getClosestPoint(Vec3 point, Vector<Vec3>& points, Index& closestID, Vec3& closestPoint);
void spaceColonization(Vector<Vec3> & points, Vector<Vec3> & branches, Vector<Index> & connections, Float segmentSize, Float killRadius);