
#include "OctreeSystemRenderer.hpp"
#include "Debug.hpp"
#include "Renderer.hpp"
#include "RelativeChunkOffset.hpp"
#include "PlanetSystemPlayer.hpp"

void OctreeNodeRenderData::count()
{
	static Int nodeCount = 0;
	if (equivalentOctreeNode->id.level == 0) {
		nodeCount = 0;
	}
	nodeCount++;
	if (subdivided) {
		c000->count();
		c001->count();
		c010->count();
		c011->count();
		c100->count();
		c101->count();
		c110->count();
		c111->count();
	}

	if (equivalentOctreeNode->id.level == 0) {
		logDebug(String("OctreeNodeRenderDataCount: ").append(toString(nodeCount)));
	}
}
void OctreeNodeRenderData::destroy()
{
	unsubdivide();
	equivalentOctreeNode->decrementUser();
}
void OctreeNodeRenderData::subdivide()
{
	if (!subdivided) {
		c000 = new OctreeNodeRenderData();
		c001 = new OctreeNodeRenderData();
		c010 = new OctreeNodeRenderData();
		c011 = new OctreeNodeRenderData();
		c100 = new OctreeNodeRenderData();
		c101 = new OctreeNodeRenderData();
		c110 = new OctreeNodeRenderData();
		c111 = new OctreeNodeRenderData();

		c000->create(*equivalentOctreeNode->c000);
		c001->create(*equivalentOctreeNode->c001);
		c010->create(*equivalentOctreeNode->c010);
		c011->create(*equivalentOctreeNode->c011);
		c100->create(*equivalentOctreeNode->c100);
		c101->create(*equivalentOctreeNode->c101);
		c110->create(*equivalentOctreeNode->c110);
		c111->create(*equivalentOctreeNode->c111);

		subdivided = true;
	}
}
void OctreeNodeRenderData::unsubdivide()
{
	if (subdivided) {
		c000->destroy();
		c001->destroy();
		c010->destroy();
		c011->destroy();
		c100->destroy();
		c101->destroy();
		c110->destroy();
		c111->destroy();

		delete c000;
		delete c001;
		delete c010;
		delete c011;
		delete c100;
		delete c101;
		delete c110;
		delete c111;

		c000 = nullptr;
		c001 = nullptr;
		c010 = nullptr;
		c011 = nullptr;
		c100 = nullptr;
		c101 = nullptr;
		c110 = nullptr;
		c111 = nullptr;

		subdivided = false;
	}
}
void OctreeNodeRenderData::render(Engine& engine, Renderer & renderer)
{
	renderer.uniforms().cameraPos() = Vec4(cameraPosition, 1);
	renderer.uniforms().worldOffset() = Vec4(equivalentOctreeNode->id.location.x, equivalentOctreeNode->id.location.y, equivalentOctreeNode->id.location.z, equivalentOctreeNode->id.level);

	if(renderer.planetRenderSystem.chunkBorders)renderer.renderMesh(engine, "gizmo");
}
void OctreeNodeRenderData::create(OctreeNode& octreeNode)
{
	equivalentOctreeNode = &octreeNode;
	octreeNode.incrementUser();
}
void OctreeNodeRenderData::renderWater(Engine& engine, Renderer &renderer){
	
	renderer.uniforms().cameraPos() = Vec4(cameraPosition, 1);
	renderer.uniforms().worldOffset() = Vec4(equivalentOctreeNode->id.location.x, equivalentOctreeNode->id.location.y, equivalentOctreeNode->id.location.z, equivalentOctreeNode->id.level);
	renderer.renderMesh(engine, "waterPlane");
}
void OctreeNodeRenderData::update(PlanetSystemPlayer& player)
{
	updateWithoutSubdivision(player);

	if (equivalentOctreeNode->subdivided && !subdivided && inDrawDistance) {
		subdivide();
		c000->updateWithoutSubdivision(player);
		c001->updateWithoutSubdivision(player);
		c010->updateWithoutSubdivision(player);
		c011->updateWithoutSubdivision(player);
		c100->updateWithoutSubdivision(player);
		c101->updateWithoutSubdivision(player);
		c110->updateWithoutSubdivision(player);
		c111->updateWithoutSubdivision(player);
	}
}

void OctreeNodeRenderData::renderLevel(Engine& engine, UShort level, Renderer &renderer)
{
	if (subdivided) {
		c000->renderLevel(engine, level, renderer);
		c001->renderLevel(engine, level, renderer);
		c010->renderLevel(engine, level, renderer);
		c011->renderLevel(engine, level, renderer);
		c100->renderLevel(engine, level, renderer);
		c101->renderLevel(engine, level, renderer);
		c110->renderLevel(engine, level, renderer);
		c111->renderLevel(engine, level, renderer);
	} else {
		if (equivalentOctreeNode->id.level == level) {
			render(engine, renderer);
		}
	}
}
void OctreeNodeRenderData::renderWaterLevel(Engine& engine, UShort level, Renderer & renderer){
	if (subdivided) {
		c000->renderWaterLevel(engine, level, renderer);
		c001->renderWaterLevel(engine, level, renderer);
		c010->renderWaterLevel(engine, level, renderer);
		c011->renderWaterLevel(engine, level, renderer);
		c100->renderWaterLevel(engine, level, renderer);
		c101->renderWaterLevel(engine, level, renderer);
		c110->renderWaterLevel(engine, level, renderer);
		c111->renderWaterLevel(engine, level, renderer);
	} else {
		if (equivalentOctreeNode->id.level == level) {
			renderWater(engine, renderer);
		}
	}
}
void OctreeNodeRenderData::updateWithoutSubdivision(PlanetSystemPlayer& player)
{
	//get camera relative location
	cameraPosition = relativeCameraPosition(equivalentOctreeNode->id, player.chunkLocation, player.camera.location);

	inDrawDistance = glm::length(cameraPosition - Vec3(0.5, 0.5, 0)) < drawDistance;

	if (!inDrawDistance) unsubdivide();

	if (subdivided) {

		c000->update(player);
		c001->update(player);
		c010->update(player);
		c011->update(player);
		c100->update(player);
		c101->update(player);
		c110->update(player);
		c111->update(player);
	}
}

void OctreeRenderSystem::count()
{
	root.count();
}
void OctreeRenderSystem::destroy() {
	root.destroy();
}
void OctreeRenderSystem::create(Renderer& renderer) {
	//[TODO]
	//waterTextureID = renderer.textureSystem.getTextureID("water");
}
void OctreeRenderSystem::update(PlanetSystemPlayer& player) {
	root.update(player);
}
void OctreeRenderSystem::renderLevel(Engine& engine, UShort level, Renderer& renderer) {
	root.renderLevel(engine, level, renderer);
}