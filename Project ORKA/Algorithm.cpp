
#include "Algorithm.hpp"

Vec3 vectorFromAToB(Vec3 a, Vec3 b) {
	return b - a;
}
void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius) {
	Vector<Vec3> available;

	for (Vec3 current : points) {
		if (distance(point, current) > radius) {
			available.push_back(current);
		}
	}

	points = available;
}
void getClosestPoint(Vec3 point, Vector<Vec3>& points, Index& closestID, Vec3& closestPoint) {

	Float minimalDistance = distance(point, points[0]);
	Index closestPointID = 0;

	for (Int i = 0; i < points.size(); i++) {
		Float currentDistance = distance(point, points[i]);
		if (currentDistance < minimalDistance) {
			minimalDistance = currentDistance;
			closestPointID = i;
		}
	}

	closestID = closestPointID;
	closestPoint = points[closestPointID];
}
void spaceColonization(Vector<Vec3>& points, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius) {
	if (points.size() == 0) return;
	if (branches.size() == 0) branches.push_back(Vec3(0));
	if (connections.size() == 0) connections.push_back(0);

	Vector<Vec3> pull(branches.size(), Vec3(0));

	//check every space
	for (Vec3& point : points) {
		Index closestID;
		Vec3 closestPoint;
		getClosestPoint(point, branches, closestID, closestPoint);
		//add pull force to branches
		pull[closestID] += vectorFromAToB(closestPoint, point);
	}

	Vector<Vec3> newBranches;

	//calculate branches
	for (int i = 0; i < pull.size(); i++) {
		if (pull[i] != Vec3(0)) {
			//calculate direction
			pull[i] = normalize(pull[i]);
			//add branch
			Vec3 newBranchSegment = branches[i] + segmentSize * pull[i];
			newBranches.push_back(newBranchSegment);
			connections.push_back(i);
		}
	}

	//kill points
	for (Vec3 node : newBranches) {
		removePointsInRadius(node, points, killRadius);
	}

	//add branches
	for (Vec3 node : newBranches) {
		branches.push_back(node);
	}
}