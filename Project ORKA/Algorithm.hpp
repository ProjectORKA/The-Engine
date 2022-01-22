
#pragma once

#include "Basics.hpp"

void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius);
void getClosestPoint(Vec3 point, Vector<Vec3>& points, Index& closestID, Vec3& closestPoint);
void spaceColonization(Vector<Vec3> & points, Vector<Vec3> & branches, Vector<Index> & connections, Float segmentSize, Float killRadius);

Bool pointInsideSphere(Vec3 point, Sphere sphere);
Bool pointInsideSpheres(Vec3 point, List<Sphere> spheres);
Bool pointInsideSpheres(Vec3 point, Vector<Sphere> spheres);
Bool pointInsideSphereAtlocationWithRadius(Vec3 point, Vec3 position, Float radius);

Float getDistanceToClosestPoint(Vec3 point, Vector<Vec3>& points);

Vec3 vectorFromAToB(Vec3 a, Vec3 b);
Vec3 getClosestPoint(Vec3 point, List<Vec3>& points);