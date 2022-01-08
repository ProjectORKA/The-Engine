
#include "TreeSystem.hpp"
#include "Renderer.hpp"

void treeGeneration(Vector<Vec3>& leaves, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius, Float leafPull) {
	if (leaves.size() == 0) return;
	if (branches.size() == 0) branches.push_back(Vec3(0));
	if (connections.size() == 0) connections.push_back(0);

	Vector<Vec3> pullBranches(branches.size(), Vec3(0));
	Vector<Vec3> pullLeaves(leaves.size(), Vec3(0));

	//check every space
	for (Int i = 0; i < leaves.size(); i++) {
		Index closestID;
		Vec3 closestPoint;
		getClosestPoint(leaves[i], branches, closestID, closestPoint);
		//add pull force to branches
		Vec3 delta = vectorFromAToB(closestPoint, leaves[i]);
		pullBranches[closestID] += delta;
		pullLeaves[i] -= delta;
	}

	Vector<Vec3> newBranches;

	//calculate branches
	for (int i = 0; i < pullBranches.size(); i++) {
		if (pullBranches[i] != Vec3(0)) {
			//calculate direction
			pullBranches[i] = normalize(pullBranches[i]) + Vec3(0.001) * pullBranches[i];
			//add branch
			Vec3 newBranchSegment = branches[i] + segmentSize * pullBranches[i];
			newBranches.push_back(newBranchSegment);
			connections.push_back(i);
		}
	}

	//calculate leaves
	for (int i = 0; i < pullLeaves.size(); i++) {
		//calculate delta
		pullLeaves[i] = pullLeaves[i] * leafPull;
		leaves[i] += pullLeaves[i];
	}

	//kill points
	for (Vec3 node : newBranches) {
		removePointsInRadius(node, leaves, killRadius);
	}

	//add branches
	for (Vec3 node : newBranches) {
		branches.push_back(node);
	}
}

void TreeGenerator::run() {
	//tree generation algorithm
	if (branches.points.size() == 0)
	{
		Float treeHeight = 2;
		Float currentBranchHeight = -1;
		for (int i = 0; i < 100; i++) {
			branches.points.push_back(Vec3(0, 0, currentBranchHeight));
			tree.connections.push_back(max(0, i - 1));
			currentBranchHeight += 0.01 * treeHeight;
			currentBranchHeight /= 1.01;
		}
	}

	//generate points
	if (leaves.points.size() == 0 && branches.points.size() < 100000 && treeGen) {
		for (int i = 0; i < 10000; i++) {
			Float height = sqrt(randomFloat());
			Vec3 p = randomPointInSphere(height) * Vec3(sqrt(randomFloat())) - Vec3(0, 0, 2 * height - 2);
			leaves.add(p);
		}
	}

	treeGeneration(leaves.points, branches.points, tree.connections, 0.01, 0.04, 0.2);
}

void TreeGenerator::render(Renderer & renderer) {
	leaves.render(renderer);
	tree.render(renderer, branches.points);
}