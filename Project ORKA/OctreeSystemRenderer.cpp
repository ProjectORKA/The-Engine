#include "OctreeSystemRenderer.hpp"
#include "Debug.hpp"
#include "Renderer.hpp"
#include "RelativeChunkOffset.hpp"
#include "PlanetSystemPlayer.hpp"

void OctreeRenderSystem::destroy()
{
	root.destroy();
}

void OctreeNodeRenderData::destroy()
{
	unsubdivide();
	equivalentOctreeNode->decrementUser();
}

void OctreeNodeRenderData::subdivide()
{
	if(!subdivided)
	{
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

void OctreeRenderSystem::count() const
{
	root.count();
}

void OctreeNodeRenderData::count() const
{
	static Int nodeCount = 0;
	if(equivalentOctreeNode->id.level == 0) nodeCount = 0;
	nodeCount++;
	if(subdivided)
	{
		c000->count();
		c001->count();
		c010->count();
		c011->count();
		c100->count();
		c101->count();
		c110->count();
		c111->count();
	}

	if(equivalentOctreeNode->id.level == 0) logDebug("OctreeNodeRenderDataCount: " + toString(nodeCount));
}

void OctreeNodeRenderData::unsubdivide()
{
	if(subdivided)
	{
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

void OctreeRenderSystem::create(Renderer& renderer) const
{
	// [TODO]
	// waterTextureId = renderer.textureSystem.getTextureId("water");
}

void OctreeRenderSystem::update(PlanetSystemPlayer& player)
{
	root.update(player);
}

void OctreeNodeRenderData::update(PlanetSystemPlayer& player)
{
	updateWithoutSubdivision(player);

	if(equivalentOctreeNode->subdivided && !subdivided && inDrawDistance)
	{
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

void OctreeNodeRenderData::updateWithoutSubdivision(PlanetSystemPlayer& player)
{
	// get camera relative position
	cameraPosition = relativeCameraPosition(equivalentOctreeNode->id, player.chunkPosition, player.camera.getPosition());

	inDrawDistance = length(cameraPosition - Vec3(0.5, 0.5, 0)) < drawDistance;

	if(!inDrawDistance) unsubdivide();

	if(subdivided)
	{
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

void OctreeNodeRenderData::render(Renderer& renderer) const
{
	renderer.uniforms().setCameraPos(Vec4(cameraPosition, 1));
	renderer.uniforms().setWorldOffset(Vec4(equivalentOctreeNode->id.position.x, equivalentOctreeNode->id.position.y, equivalentOctreeNode->id.position.z, equivalentOctreeNode->id.level));

	if(renderer.planetRenderSystem.chunkBorders) renderer.renderMesh("gizmo");
}

void OctreeNodeRenderData::renderWater(Renderer& renderer) const
{
	renderer.uniforms().setCameraPos(Vec4(cameraPosition, 1));
	renderer.uniforms().setWorldOffset(Vec4(equivalentOctreeNode->id.position.x, equivalentOctreeNode->id.position.y, equivalentOctreeNode->id.position.z, equivalentOctreeNode->id.level));
	renderer.renderMesh("waterPlane");
}

void OctreeRenderSystem::renderLevel(const UShort level, Renderer& renderer) const
{
	root.renderLevel(level, renderer);
}

void OctreeNodeRenderData::renderLevel(const UShort level, Renderer& renderer) const
{
	if(subdivided)
	{
		c000->renderLevel(level, renderer);
		c001->renderLevel(level, renderer);
		c010->renderLevel(level, renderer);
		c011->renderLevel(level, renderer);
		c100->renderLevel(level, renderer);
		c101->renderLevel(level, renderer);
		c110->renderLevel(level, renderer);
		c111->renderLevel(level, renderer);
	}
	else
	{
		if(equivalentOctreeNode->id.level == level) render(renderer);
	}
}

void OctreeNodeRenderData::renderWaterLevel(const UShort level, Renderer& renderer) const
{
	if(subdivided)
	{
		c000->renderWaterLevel(level, renderer);
		c001->renderWaterLevel(level, renderer);
		c010->renderWaterLevel(level, renderer);
		c011->renderWaterLevel(level, renderer);
		c100->renderWaterLevel(level, renderer);
		c101->renderWaterLevel(level, renderer);
		c110->renderWaterLevel(level, renderer);
		c111->renderWaterLevel(level, renderer);
	}
	else
	{
		if(equivalentOctreeNode->id.level == level) renderWater(renderer);
	}
}
