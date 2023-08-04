#pragma once

#include "QuadtreeNode.hpp"

struct QuadtreeSystem
{
	QuadtreeNode* root = nullptr;

	void          count() const;
	void          destroy() const;
	void          update(TerrainSystem& terrainSystem);
	void          create(const TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, const QuadtreeId& id) const;
};
