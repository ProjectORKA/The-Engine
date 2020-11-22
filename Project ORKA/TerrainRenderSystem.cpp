
#include "TerrainRenderSystem.hpp"

void TerrainRenderInfo::render()
{
}
void TerrainRenderInfo::subdivide()
{
	if (!subdivided) {

		tri00 = new TerrainRenderInfo();
		tri01 = new TerrainRenderInfo();
		tri10 = new TerrainRenderInfo();
		tri11 = new TerrainRenderInfo();
		tri00->level = level + 1;
		tri01->level = level + 1;
		tri10->level = level + 1;
		tri11->level = level + 1;

		subdivided = true;
	}
}
void TerrainRenderInfo::update(WorldChunk& chunk)
{
	if (level == chunk.level) {
		create(chunk);
	}
	else {
		logDebug("important data:");
		logDebug(std::bitset<64>(chunk.xLocation >> 62 - level));
		logDebug(std::bitset<64>(chunk.yLocation >> 62 - level));
	}
}
void TerrainRenderInfo::create(WorldChunk & chunk) {
	CPUMesh mesh;
	mesh.generate2DTerrainMesh(chunk.terrain);
	terrainMesh.upload(mesh);
}

void TerrainRenderSystem::create(WorldChunk& chunk)
{
	if (chunk.level == 0) {
		terrainRenderInfo.create(chunk);
	}
	else {
		logError("TerrainRenderSystem not initialized with root chunk.");
	}
}
void TerrainRenderSystem::update(WorldChunk& chunk)
{
	terrainRenderInfo.update(chunk);
}