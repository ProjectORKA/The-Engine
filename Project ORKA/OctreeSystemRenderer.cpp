#include "OctreeSystemrenderer.hpp"
#include "Renderer.hpp"

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
		logDebug(String("OctreeNodeRenderDataCount: ").append(std::to_string(nodeCount)));
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
void OctreeNodeRenderData::renderWater(Renderer &renderer){
	
	renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4(chunkOffset, 1);
	renderer.shaderSystem.uniforms.data.worldOffset = Vec4(equivalentOctreeNode->id.location.x, equivalentOctreeNode->id.location.y, equivalentOctreeNode->id.location.z, equivalentOctreeNode->id.level);
	renderer.shaderSystem.uniforms.update();

	renderer.meshSystem.render("waterPlane");
}
void OctreeNodeRenderData::update(PlanetCamera& camera)
{
	updateWithoutSubdivision(camera);

	if (equivalentOctreeNode->subdivided && !subdivided && inDrawDistance) {
		subdivide();
		c000->updateWithoutSubdivision(camera);
		c001->updateWithoutSubdivision(camera);
		c010->updateWithoutSubdivision(camera);
		c011->updateWithoutSubdivision(camera);
		c100->updateWithoutSubdivision(camera);
		c101->updateWithoutSubdivision(camera);
		c110->updateWithoutSubdivision(camera);
		c111->updateWithoutSubdivision(camera);
	}
}
void OctreeNodeRenderData::create(OctreeNode& octreeNode)
{
	equivalentOctreeNode = &octreeNode;
	octreeNode.incrementUser();
}
void OctreeNodeRenderData::render(Renderer & renderer)
{
	renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4(chunkOffset,1);
	renderer.shaderSystem.uniforms.data.worldOffset = Vec4(equivalentOctreeNode->id.location.x, equivalentOctreeNode->id.location.y, equivalentOctreeNode->id.location.z, equivalentOctreeNode->id.level);
	renderer.shaderSystem.uniforms.update();

	if(renderer.planetRenderSystem.chunkBorders)renderer.meshSystem.render("gizmo");
}

void OctreeNodeRenderData::renderLevel(UShort level, Renderer &renderer)
{
	if (subdivided) {
		c000->renderLevel(level, renderer);
		c001->renderLevel(level, renderer);
		c010->renderLevel(level, renderer);
		c011->renderLevel(level, renderer);
		c100->renderLevel(level, renderer);
		c101->renderLevel(level, renderer);
		c110->renderLevel(level, renderer);
		c111->renderLevel(level, renderer);
	} else {
		if (equivalentOctreeNode->id.level == level) {
			render(renderer);
		}
	}
}
void OctreeNodeRenderData::updateWithoutSubdivision(PlanetCamera& camera)
{
	//get camera relative location
	chunkOffset = cameraRelativeLocationOfChunk(equivalentOctreeNode->id, camera);

	inDrawDistance = glm::length(chunkOffset + Vec3(0.5, 0.5, 0)) < drawDistance;

	if (!inDrawDistance) unsubdivide();

	if (subdivided) {

		c000->update(camera);
		c001->update(camera);
		c010->update(camera);
		c011->update(camera);
		c100->update(camera);
		c101->update(camera);
		c110->update(camera);
		c111->update(camera);
	}
}
void OctreeNodeRenderData::renderWaterLevel(UShort level, Renderer & renderer){
	if (subdivided) {
		c000->renderWaterLevel(level, renderer);
		c001->renderWaterLevel(level, renderer);
		c010->renderWaterLevel(level, renderer);
		c011->renderWaterLevel(level, renderer);
		c100->renderWaterLevel(level, renderer);
		c101->renderWaterLevel(level, renderer);
		c110->renderWaterLevel(level, renderer);
		c111->renderWaterLevel(level, renderer);
	} else {
		if (equivalentOctreeNode->id.level == level) {
			renderWater(renderer);
		}
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
	waterTextureID = renderer.textureSystem.getTextureID("water");
}
void OctreeRenderSystem::update(PlanetCamera& camera) {
	root.update(camera);
}
void OctreeRenderSystem::renderLevel(UShort level, Renderer& renderer) {
	root.renderLevel(level, renderer);
}