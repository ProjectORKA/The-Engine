#include "TreeSystem.hpp"
#include "Renderer.hpp"

void treeGeneration(Vector<Vec3>& leaves, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize,
                    Float killRadius, Float leafPull) {
	if (leaves.empty()) return;
	if (branches.empty()) branches.push_back(Vec3(0));
	if (connections.empty()) connections.push_back(0);

	Vector<Vec3> pullBranches(branches.size(), Vec3(0));
	Vector<Vec3> pullLeaves(leaves.size(), Vec3(0));

	//check every space
	for (Int i = 0; i < leaves.size(); i++) {
		Index closestId;
		Vec3 closestPoint;
		getClosestPoint(leaves[i], branches, closestId, closestPoint);
		//add pull force to branches
		const Vec3 delta = vectorFromAToB(closestPoint, leaves[i]);
		pullBranches[closestId] += delta;
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
	for (const Vec3 node : newBranches) { removePointsInRadius(node, leaves, killRadius); }

	//add branches
	for (Vec3 node : newBranches) { branches.push_back(node); }
}

void TreeGenerator::run() {
	//tree generation algorithm
	if (branches.points.empty()) {
		const Float treeHeight = 2;
		Float currentBranchHeight = -1;
		for (int i = 0; i < 100; i++) {
			branches.points.push_back(Vec3(0, 0, currentBranchHeight));
			tree.connections.push_back(max(0, i - 1));
			currentBranchHeight += 0.01 * treeHeight;
			currentBranchHeight /= 1.01;
		}
	}

	//generate points
	if (leaves.points.empty() && branches.points.size() < 100000 && treeGen) {
		for (int i = 0; i < 10000; i++) {
			const Float height = sqrt(randomFloat());
			const Vec3 p = randomPointInSphere(height) * Vec3(sqrt(randomFloat())) - Vec3(0, 0, 2 * height - 2);
			leaves.add(p);
		}
	}

	treeGeneration(leaves.points, branches.points, tree.connections, 0.01, 0.04, 0.2);
}

void TreeGenerator::render(Renderer& renderer) {
	PointCloudRenderer p;
	p.render(leaves, renderer);
	tree.render(renderer, branches.points);
}
