#pragma once

#include "Basics.hpp"

struct KDTreeNode
{
	ULL a       = maxULL;
	ULL b       = maxULL;
	ULL parent  = maxULL;
	ULL pointId = 0;
};

struct KDTree
{
	Vec3Vector*      points;
	Vector<KDTreeNode> nodes;

	KDTree() = delete;

	explicit KDTree(Vec3Vector& points)
	{
		this->points = &points;
		nodes.emplace_back();
		for(ULL i = 0; i < points.size(); i++) addPoint(i);
	}

	void addPoint(const ULL pointID)
	{
		injectPointInNode(0, pointID, 0);
	}

	[[nodiscard]] constexpr Float squaredDistance(const Vec3& p1, const Vec3& p2) const
	{
		const Float dx = p1.x - p2.x;
		const Float dy = p1.y - p2.y;
		const Float dz = p1.z - p2.z;
		return dx * dx + dy * dy + dz * dz;
	}

	[[nodiscard]] ULL getClosestPointId(const Vec3& queryPoint) const
	{
		ULL    closestPointId  = maxULL;
		Float closestDistance = std::numeric_limits<Float>::max();

		// Recursive function to traverse the KD tree and find the closest point
		std::function<void(ULL, Byte)> searchNearest = [&](const ULL nodeId, const Byte axis)
		{
			if(nodeId == maxULL) return;

			const KDTreeNode& node         = nodes[nodeId];
			const Vec3&       nodePoint    = (*points)[node.pointId];
			const Float      nodeDistance = squaredDistance(queryPoint, nodePoint);

			if(nodeDistance < closestDistance)
			{
				closestDistance = nodeDistance;
				closestPointId  = node.pointId;
			}

			// Calculate the axis-aligned distance from the query point to the current node
			Float axisDistance = 0.0;
			if(axis == 0)
			{
				axisDistance = queryPoint.x - nodePoint.x;
			}
			else if(axis == 1)
			{
				axisDistance = queryPoint.y - nodePoint.y;
			}
			else
			{
				axisDistance               = queryPoint.z - nodePoint.z;
			}

			// Choose the next child node to explore
			const ULL  nextNodeId = axisDistance >= 0 ? node.b : node.a;
			const Byte nextAxis   = (axis + 1) % 3;

			// Recursively search both subtrees
			searchNearest(nextNodeId, nextAxis);

			// Check if we need to search the other subtree based on distance
			if(axisDistance * axisDistance < closestDistance)
			{
				searchNearest(axisDistance >= 0 ? node.a : node.b, nextAxis);
			}
		};

		// Start the search from the root node
		searchNearest(0, 0);

		return closestPointId;
	}

	void injectPointInNode(const ULL nodeID, const ULL pointId, const Byte axis)
	{
		const Vec3 newPoint   = (*points)[pointId];
		const Vec3 comparison = (*points)[nodeID];
		KDTreeNode newNode;

		if(axis == 0)
		{
			if(newPoint.x >= comparison.x)
			{
				if(nodes[nodeID].b == maxULL)
				{
					newNode.parent  = nodeID;
					newNode.pointId = pointId;
					nodes[nodeID].b = nodes.size();
					nodes.push_back(newNode);
					return;
				}
				else
				{
					injectPointInNode(nodes[nodeID].b, pointId, axis + 1);
					return;
				}
			}
			else
			{
				if(nodes[nodeID].a == maxULL)
				{
					newNode.parent  = nodeID;
					newNode.pointId = pointId;
					nodes[nodeID].a = nodes.size();
					nodes.push_back(newNode);
					return;
				}
				else
				{
					injectPointInNode(nodes[nodeID].a, pointId, axis + 1);
					return;
				}
			}
			return;
		}
		if(axis == 1)
		{
			if(newPoint.y >= comparison.y)
			{
				if(nodes[nodeID].b == maxULL)
				{
					newNode.parent  = nodeID;
					newNode.pointId = pointId;
					nodes[nodeID].b = nodes.size();
					nodes.push_back(newNode);
					return;
				}
				else
				{
					injectPointInNode(nodes[nodeID].b, pointId, axis + 1);
					return;
				}
			}
			else
			{
				if(nodes[nodeID].a == maxULL)
				{
					newNode.parent  = nodeID;
					newNode.pointId = pointId;
					nodes[nodeID].a = nodes.size();
					nodes.push_back(newNode);
					return;
				}
				else
				{
					injectPointInNode(nodes[nodeID].a, pointId, axis + 1);
					return;
				}
			}
			return;
		}
		if(newPoint.z >= comparison.z)
		{
			if(nodes[nodeID].b == maxULL)
			{
				newNode.parent  = nodeID;
				newNode.pointId = pointId;
				nodes[nodeID].b = nodes.size();
				nodes.push_back(newNode);
				return;
			}
			else
			{
				injectPointInNode(nodes[nodeID].b, pointId, 0);
				return;
			}
		}
		else
		{
			if(nodes[nodeID].a == maxULL)
			{
				newNode.parent  = nodeID;
				newNode.pointId = pointId;
				nodes[nodeID].a = nodes.size();
				nodes.push_back(newNode);
				return;
			}
			else
			{
				injectPointInNode(nodes[nodeID].a, pointId, 0);
				return;
			}
		}
	}
};
