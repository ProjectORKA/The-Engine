#include "TreeSystem.hpp"
#include "Renderer.hpp"

void TreeGenerator::run()
{
	// tree generation algorithm
	if(branches.empty())
	{
		const Float treeHeight          = 2;
		Float       currentBranchHeight = -1;
		for(int i = 0; i < 100; i++)
		{
			branches.emplace_back(0, 0, currentBranchHeight);
			tree.connections.push_back(max(0, i - 1));
			currentBranchHeight += 0.01f * treeHeight;
			currentBranchHeight /= 1.01f;
		}
	}

	// generate points
	if(leaves.empty() && branches.size() < 100000 && treeGen)
	{
		for(int i = 0; i < 10000; i++)
		{
			const Float height = sqrt(randomFloatFast());
			const Vec3  p      = randomPointInSphereFast(height) * Vec3(sqrt(randomFloatFast())) - Vec3(0, 0, 2 * height - 2);
			leaves.push_back(p);
		}
	}

	treeGeneration(leaves, branches, tree.connections, 0.01f, 0.04f, 0.2f);
}

void TreeGenerator::render(Renderer& renderer) const
{
	renderer.points(leaves);
	tree.render(renderer, branches);
}

void treeGeneration(Vec3Vector& leaves, Vec3Vector& branches, Vector<Index>& connections, const Float segmentSize, const Float killRadius, const Float leafPull)
{
	if(leaves.empty()) return;
	if(branches.empty()) branches.emplace_back(0);
	if(connections.empty()) connections.push_back(0);

	Vec3Vector pullBranches(branches.size(), Vec3(0));
	Vec3Vector pullLeaves(leaves.size(), Vec3(0));

	// check every space
	for(Int i = 0; i < leaves.size(); i++)
	{
		Index closestId;
		Vec3  closestPoint;
		getClosestPoint(leaves[i], branches, closestId, closestPoint);
		// add pull force to branches
		const Vec3 delta = vectorFromAToB(closestPoint, leaves[i]);
		pullBranches[closestId] += delta;
		pullLeaves[i] -= delta;
	}

	Vec3Vector newBranches;

	// calculate branches
	for(int i = 0; i < pullBranches.size(); i++)
	{
		if(pullBranches[i] != Vec3(0))
		{
			// calculate direction
			pullBranches[i] = normalize(pullBranches[i]) + Vec3(0.001f) * pullBranches[i];
			// add branch
			Vec3 newBranchSegment = branches[i] + segmentSize * pullBranches[i];
			newBranches.push_back(newBranchSegment);
			connections.push_back(i);
		}
	}

	// calculate leaves
	for(int i = 0; i < pullLeaves.size(); i++)
	{
		// calculate delta
		pullLeaves[i] = pullLeaves[i] * leafPull;
		leaves[i] += pullLeaves[i];
	}

	// kill points
	for(const Vec3 node : newBranches) removePointsInRadius(node, leaves, killRadius);

	// add branches
	for(Vec3 node : newBranches) branches.push_back(node);
}
