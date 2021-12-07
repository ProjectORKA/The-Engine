
#include "QuadtreeSystemRenderer.hpp"
#include "Renderer.hpp"

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
void QuadtreeNodeRenderData::loadTerrainMesh() {
	if (!terrainMesh.loaded && equivalentQuadtreeNode->data.terrain) {
		CPUMesh cpuMesh;
		cpuMesh = createTerrainMesh(equivalentQuadtreeNode->data.terrain->heightmapForNormals);
		terrainMesh.upload(cpuMesh);
	}
}
void QuadtreeNodeRenderData::update(PlanetCamera& camera)
{
	updateWithoutSubdivision(camera);
}
void QuadtreeNodeRenderData::renderTerrain(Renderer& renderer)
{
	if (equivalentQuadtreeNode->id.level < 4) { //for the higher level chunks we duplicated them 
		for (Int x = -1; x < 2; x++) {
			for (Int y = -1; y < 2; y++) {

				loadTerrainMesh();

				renderer.uniforms().data.chunkOffsetVector = Vec4(chunkOffset + Vec3(x * pow(2, equivalentQuadtreeNode->id.level), y * pow(2, equivalentQuadtreeNode->id.level), 0), 1);
				renderer.uniforms().data.worldOffset = Vec4(equivalentQuadtreeNode->id.location.x, equivalentQuadtreeNode->id.location.y, equivalentQuadtreeNode->data.terrain->lowerLimit, equivalentQuadtreeNode->id.level);
				renderer.uniforms().update();
				
				terrainMesh.render();
			}
		}
	}
	else {
		loadTerrainMesh();
		
		renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4(chunkOffset, 1);
		renderer.shaderSystem.uniforms.data.worldOffset = Vec4(equivalentQuadtreeNode->id.location.x, equivalentQuadtreeNode->id.location.y, equivalentQuadtreeNode->data.terrain->lowerLimit, equivalentQuadtreeNode->id.level);
		renderer.shaderSystem.uniforms.update();
		terrainMesh.render();
	}
}
void QuadtreeNodeRenderData::create(QuadtreeNode& quadtreeNode)
{
	equivalentQuadtreeNode = &quadtreeNode;

	quadtreeNode.incrementUsers();
}
void QuadtreeNodeRenderData::updateWithoutSubdivision(PlanetCamera& camera)
{
	//create 3d chunk id
	OctreeID tmp;

	QuadtreeNode& eq = *equivalentQuadtreeNode;

	tmp.level = eq.id.level;
	tmp.location.x = eq.id.location.x;
	tmp.location.y = eq.id.location.y;

	if (eq.data.terrain)tmp.location.z = eq.data.terrain->lowerLimit;
	else tmp.location.z = 0;

	//get camera relative location
	chunkOffset = cameraRelativeLocationOfChunk(tmp, camera.chunkLocation, camera.location);

	nodeDrawDistance = glm::length(chunkOffset + Vec3(0.5, 0.5, 0));
	Bool inDrawDistance = nodeDrawDistance < drawDistance;
	Bool notCulled = (dot(normalize(chunkOffset + Vec3(0.5, 0.5, 0)), camera.forwardVector) > 0.1) || (nodeDrawDistance < 1.5);

	drawn = inDrawDistance & notCulled;

	if (!drawn) unsubdivide();

	if (subdivided) {
		c00->updateWithoutSubdivision(camera);
		c01->updateWithoutSubdivision(camera);
		c10->updateWithoutSubdivision(camera);
		c11->updateWithoutSubdivision(camera);
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
void QuadtreeNodeRenderData::renderTerrainLevel(UShort level, Renderer& renderer)
{
	if (subdivided) {
		c00->renderTerrainLevel(level, renderer);
		c01->renderTerrainLevel(level, renderer);
		c10->renderTerrainLevel(level, renderer);
		c11->renderTerrainLevel(level, renderer);
	}
	else {
		if (equivalentQuadtreeNode->id.level == level) {
			renderTerrain(renderer);
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
void QuadtreeRenderSystem::update(PlanetCamera& camera) {
	
	if (needsSubdiv) {
		needsSubdiv->subdivide();
		needsSubdiv = nullptr;
	}
	
	root.update(camera);
}
void QuadtreeRenderSystem::renderLevel(UShort level, Renderer& renderer) {
	root.renderTerrainLevel(level, renderer);
}