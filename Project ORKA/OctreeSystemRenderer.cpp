#include "OctreeSystemrenderer.hpp"

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
		logDebug(String("OctreeNodeCount  : ").append(std::to_string(nodeCount)));
	}
}

void OctreeNodeRenderData::destroy()
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
	}

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
void OctreeNodeRenderData::create(OctreeNode& octreeNode)
{
	equivalentOctreeNode = &octreeNode;
	octreeNode.incrementUser();
}
void OctreeNodeRenderData::update(PlanetCamera& camera, Float& renderDistance)
{
	chunkOffset = cameraRelativeLocationOfChunk(equivalentOctreeNode->id, camera);

	Bool inDrawDistance = glm::length(chunkOffset + Vec3(0.5)) < renderDistance;

	if (!inDrawDistance) unsubdivide();

	if (subdivided) {
		c000->update(camera, renderDistance);
		c001->update(camera, renderDistance);
		c010->update(camera, renderDistance);
		c011->update(camera, renderDistance);
		c100->update(camera, renderDistance);
		c101->update(camera, renderDistance);
		c110->update(camera, renderDistance);
		c111->update(camera, renderDistance);
	}

	if (equivalentOctreeNode->subdivided & !subdivided & inDrawDistance & equivalentOctreeNode->data.hasContent) {
		subdivide();
		Float fakeDistance = 0; //<-updates the new chunks but prevents them from subdividing in the same frame
		c000->update(camera, fakeDistance);
		c001->update(camera, fakeDistance);
		c010->update(camera, fakeDistance);
		c011->update(camera, fakeDistance);
		c100->update(camera, fakeDistance);
		c101->update(camera, fakeDistance);
		c110->update(camera, fakeDistance);
		c111->update(camera, fakeDistance);
	}
}
void OctreeNodeRenderData::render(Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem)
{
	shaderSystem.uniforms.data.chunkOffsetVector = Vec4(chunkOffset,1);
	shaderSystem.uniforms.data.worldOffset = Vec4(equivalentOctreeNode->id.location.x, equivalentOctreeNode->id.location.y, equivalentOctreeNode->id.location.z, equivalentOctreeNode->id.level);
	shaderSystem.uniforms.update();

	if(chunkBorders)meshSystem.renderMesh("boundingBox");
}
void OctreeNodeRenderData::renderAll(Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem)
{
	if (subdivided) {
		c000->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c001->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c010->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c011->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c100->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c101->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c110->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
		c111->renderAll(chunkBorders, meshSystem, textureSystem, shaderSystem);
	}
	else {
		render(chunkBorders, meshSystem, textureSystem, shaderSystem);
	}
}
void OctreeNodeRenderData::renderLevel(UShort level, Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem)
{
	if (subdivided) {
		c000->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c001->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c010->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c011->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c100->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c101->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c110->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
		c111->renderLevel(level, chunkBorders, meshSystem, textureSystem, shaderSystem);
	}
	else {
		if (equivalentOctreeNode->id.level == level) {
			render(chunkBorders, meshSystem, textureSystem, shaderSystem);
		}
	}
}

void OctreeRenderSystem::count()
{
	root.count();
}
