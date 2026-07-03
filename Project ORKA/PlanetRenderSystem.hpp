#pragma once
#include "GPUMesh.hpp"
#include "QuadtreeNode.hpp"

struct Framebuffer;
struct PlanetSystem;
struct PlanetSystemPlayer;

struct QuadtreeNodeRenderData
{
	GPUMesh       terrainMesh;
	Vec3          cameraPosition         = Vec3(0);
	QuadtreeNode* equivalentQuadtreeNode = nullptr;
	Float         nodeDrawDistance       = 0.0;
	Index         c00                    = -1;
	Index         c01                    = -1;
	Index         c10                    = -1;
	Index         c11                    = -1;
	Bool          isValid                = false;
	Bool          drawn                  = false;
	Bool          subdivided             = false;
};

struct PlanetRenderSystem
{
	// quadtree
	Deque<QuadtreeNodeRenderData> nodes[MAX_CHUNK_LEVEL];
	QuadtreeNodeRenderData* needsSubDiv = nullptr;
	UShort                  nodeCounts[MAX_CHUNK_LEVEL];
	Bool                    chunkBorders    = true;
	Bool                    worldDistortion = true;
	Bool                    vertexColors    = false;
	Bool                    chunkBoundingBoxes = false;

	void  count() const;
	void  destroy(Renderer& renderer);
	Index addNode(QuadtreeNode& node);
	void  create(const PlanetSystem& planetSystem);
	void  subdivideNode(QuadtreeNodeRenderData& renderData);
	void  update(Renderer& renderer, const PlanetSystemPlayer& player);
	void  destroyNode(QuadtreeNodeRenderData& renderData, Renderer& renderer);
	void  unsubdivideNode(QuadtreeNodeRenderData& renderData, Renderer& renderer);
	void  renderNode(QuadtreeNodeRenderData& renderData, Renderer& renderer) const;
	void  createNode(QuadtreeNodeRenderData& renderData, QuadtreeNode& quadtreeNode);
	void  updateNode(QuadtreeNodeRenderData& renderData, Renderer& renderer, const PlanetSystemPlayer& player);
	void  renderLevel(Renderer& renderer, const PlanetSystemPlayer& player, UShort level, const Framebuffer& framebuffer);
};
