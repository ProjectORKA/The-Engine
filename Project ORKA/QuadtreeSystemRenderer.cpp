
#include "QuadtreeSystemRenderer.hpp"
#include "Renderer.hpp"
#include "PlanetSystemPlayer.hpp"
#include "RelativeChunkOffset.hpp"
#include "TerrainRendering.hpp"

QuadtreeNodeRenderData* needsSubdiv = nullptr;

void QuadtreeNodeRenderData::count()
{
	static Int nodeCount = 0;
	if (equivalentQuadtreeNode->id.level == 0) {
		nodeCount = 0;
	}
	nodeCount++;
	if (subdivided) {
		c00->count();
		c01->count();
		c10->count();
		c11->count();
	}
	if (equivalentQuadtreeNode->id.level == 0) {
		logDebug(String("QuadTreeNodeRenderData count: ").append(std::to_string(nodeCount)));
	}
}
void QuadtreeNodeRenderData::destroy()
{
	unsubdivide();
	terrainMesh.unload();
	equivalentQuadtreeNode->decrementUsers();
}
void QuadtreeNodeRenderData::subdivide()
{
	if (!subdivided) {
		c00 = new QuadtreeNodeRenderData();
		c01 = new QuadtreeNodeRenderData();
		c10 = new QuadtreeNodeRenderData();
		c11 = new QuadtreeNodeRenderData();

		c00->create(*equivalentQuadtreeNode->c00);
		c01->create(*equivalentQuadtreeNode->c01);
		c10->create(*equivalentQuadtreeNode->c10);
		c11->create(*equivalentQuadtreeNode->c11);

		//mesh.clearGeometry();
		//mesh.merge(c00->mesh);
		//mesh.merge(c01->mesh);
		//mesh.merge(c10->mesh);
		//mesh.merge(c11->mesh);
		//mesh.removeDoubles();
		//mesh.calculateSmoothNormals();
		//terrainMesh.unload();
		//terrainMesh.upload(mesh);

		subdivided = true;
	}
}
void QuadtreeNodeRenderData::unsubdivide()
{
	if (subdivided) {
		c00->destroy();
		c01->destroy();
		c10->destroy();
		c11->destroy();

		delete c00;
		delete c01;
		delete c10;
		delete c11;

		c00 = nullptr;
		c01 = nullptr;
		c10 = nullptr;
		c11 = nullptr;

		subdivided = false;
	}
}
void QuadtreeNodeRenderData::renderTerrainTile(Renderer& renderer) {
	QuadtreeNode& node = *equivalentQuadtreeNode;

	renderer.uniforms().mMatrix(Matrix(1));
	renderer.uniforms().worldOffset() = Vec4(node.id.location.x, node.id.location.y, node.data.terrainTile.height, node.id.level);

	if (node.id.level < 4) { //for the higher level chunks we duplicated them 
		for (Int x = -1; x < 2; x++) {
			for (Int y = -1; y < 2; y++) {
				renderer.uniforms().cameraPos() = Vec4(terrainTileCameraPosition - Vec3(x * pow(2, node.id.level), y * pow(2, node.id.level), 0), 1);
				terrainMesh.render(renderer.uniforms());
			}
		}
	}
	else {
		renderer.uniforms().cameraPos() = Vec4(terrainTileCameraPosition, 1);
		terrainMesh.render(renderer.uniforms());
	}
}
void QuadtreeNodeRenderData::create(QuadtreeNode& quadtreeNode)
{
	equivalentQuadtreeNode = &quadtreeNode;
	quadtreeNode.incrementUsers();
	createMesh();
}
void QuadtreeNodeRenderData::update(PlanetSystemPlayer& player)
{
	updateWithoutSubdivision(player);
}
void QuadtreeNodeRenderData::updateWithoutSubdivision(PlanetSystemPlayer& player)
{
	//create 3d chunk id
	OctreeID tmp1, tmp2;

	tmp1.level = equivalentQuadtreeNode->id.level;
	tmp1.location.x = equivalentQuadtreeNode->id.location.x;
	tmp1.location.y = equivalentQuadtreeNode->id.location.y;

	if (equivalentQuadtreeNode->data.terrain)tmp1.location.z = ULL(equivalentQuadtreeNode->data.terrain->heightmap.lowerLimit);
	else tmp1.location.z = 0;

	tmp2 = tmp1;

	tmp2.location.z = equivalentQuadtreeNode->data.terrainTile.height + LDouble(SEALEVEL);

	//get camera relative location
	terrainCameraPosition = relativeCameraPosition(tmp1, player.chunkLocation, player.camera.location);
	terrainTileCameraPosition = relativeCameraPosition(tmp2, player.chunkLocation, player.camera.location);

	//take minimum of both distances
	//nodeDrawDistance = glm::length(terrainCameraPosition - Vec3(0.5, 0.5, 0)); //mesh terrain
	nodeDrawDistance = glm::length(terrainTileCameraPosition - Vec3(0.5, 0.5, 0));
	Bool inDrawDistance = nodeDrawDistance < drawDistance;
	//Bool notCulled = (dot(normalize(Vec3(0.5, 0.5, 0) - cameraPosition), player.camera.forwardVector) > 0.1) || (nodeDrawDistance < 1.5);

	drawn = inDrawDistance;

	if (!drawn) unsubdivide();

	if (subdivided) {
		c00->updateWithoutSubdivision(player);
		c01->updateWithoutSubdivision(player);
		c10->updateWithoutSubdivision(player);
		c11->updateWithoutSubdivision(player);
	}
	else {
		if (equivalentQuadtreeNode->subdivided && !subdivided && drawn) {
			if (needsSubdiv) {
				if (needsSubdiv->nodeDrawDistance > nodeDrawDistance) needsSubdiv = this;
			}
			else {
				needsSubdiv = this;
			}
		}
	}
}
void QuadtreeNodeRenderData::renderTerrainTileLevel(UShort level, Renderer& renderer) {
	if (subdivided) {
		c00->renderTerrainTileLevel(level, renderer);
		c01->renderTerrainTileLevel(level, renderer);
		c10->renderTerrainTileLevel(level, renderer);
		c11->renderTerrainTileLevel(level, renderer);
	}
	else {
		if (equivalentQuadtreeNode->id.level == level) {
			renderTerrainTile(renderer);
		}
	}
}

void QuadtreeRenderSystem::count()
{
	root.count();
}
void QuadtreeRenderSystem::destroy() {
	root.destroy();
}
void QuadtreeRenderSystem::update(PlanetSystemPlayer& player) {
	//this system makes sure that not too much terrain data is loaded per frame, in order to prevent stuttering
	if (needsSubdiv) {
		needsSubdiv->subdivide();
		needsSubdiv = nullptr;
	}
	root.update(player);
}
void QuadtreeRenderSystem::renderTerrainTileLevel(UShort level, Renderer& renderer) {
	root.renderTerrainTileLevel(level, renderer);
}