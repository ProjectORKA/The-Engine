#pragma once

#include "GPUMesh.hpp"
#include "QuadtreeSystem.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct QuadtreeNodeRenderData {
	GPUMesh terrainMesh;
	
	CPUMesh mesh;
	//ULL meshOffset = 0;

	Vec3 terrainCameraPosition = Vec3(0);
	Vec3 terrainTileCameraPosition = Vec3(0);

	Bool drawn = false;
	Float nodeDrawDistance = 0.0;

	QuadtreeNode * equivalentQuadtreeNode = nullptr;
	Bool subdivided = false;
	QuadtreeNodeRenderData * c00 = nullptr;
	QuadtreeNodeRenderData * c01 = nullptr;
	QuadtreeNodeRenderData * c10 = nullptr;
	QuadtreeNodeRenderData * c11 = nullptr;

	void count();
	void destroy();
	void subdivide();
	void unsubdivide();
	//void loadTerrainMesh();
	//void renderTerrain(Renderer& renderer);
	void update(PlanetSystemPlayer & player);
	void create(QuadtreeNode & quadtreeNode);
	void updateWithoutSubdivision(PlanetSystemPlayer& player);
	//void renderTerrainLevel(UShort level, Renderer &renderer);

	void renderTerrainTile(Renderer& renderer);
	void renderTerrainTileLevel(UShort level, Renderer& renderer);

	//mesh generation
	void createMesh(){
		if (!mesh.loaded) {
			//create a 2D array for height data
			Array2D<Float> data = Array2D<Float>(2);
			
			//get height from quadtree structure
			Double o = equivalentQuadtreeNode->data.terrainTile.height;
			Double e = equivalentQuadtreeNode->ner().data.terrainTile.height;
			Double n = equivalentQuadtreeNode->nnr().data.terrainTile.height;
			Double ne = equivalentQuadtreeNode->nner().data.terrainTile.height;
			
			data.set(0, 0, 0);
			data.set(1, 0, (e - o) / pow(2, 64 - equivalentQuadtreeNode->id.level));
			data.set(0, 1, (n - o) / pow(2, 64 - equivalentQuadtreeNode->id.level));
			data.set(1, 1, (ne - o) / pow(2, 64 - equivalentQuadtreeNode->id.level));

			mesh.triangleMeshFromHeightmap(data);

			mesh.removeDoubles();
			mesh.calculateSmoothNormals();

			mesh.loaded = true;

			terrainMesh.upload(mesh);
		}
		else {
			logError("Mesh already generated!");
		}
	}
};

struct QuadtreeRenderSystem {
	QuadtreeNodeRenderData root;

	void count();
	void destroy();
	void update(PlanetSystemPlayer& player);
	//void renderTerrainLevel(UShort level, Renderer& renderer);
	void renderTerrainTileLevel(UShort level, Renderer& renderer);
};