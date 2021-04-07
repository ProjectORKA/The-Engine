
#include "QuadtreeSystemRenderer.hpp"

void QuadtreeNodeRenderData::destroy()
{
	if (subdivided) {
		unsubdivide();
	}

	terrainMesh.unload();
	//heightmap.unload();

	parentNode->decrementUsers();
}
void QuadtreeNodeRenderData::subdivide()
{
	if (!subdivided) {
		c00 = std::make_shared<QuadtreeNodeRenderData>();
		c01 = std::make_shared<QuadtreeNodeRenderData>();
		c10 = std::make_shared<QuadtreeNodeRenderData>();
		c11 = std::make_shared<QuadtreeNodeRenderData>();

		c00->create(parentNode->c00);
		c01->create(parentNode->c01);
		c10->create(parentNode->c10);
		c11->create(parentNode->c11);

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

		c00.reset();
		c01.reset();
		c10.reset();
		c11.reset();

		subdivided = false;
	}
}
void QuadtreeNodeRenderData::update(PlanetCamera& camera)
{
	updateWithoutSubdivision(camera);

	if (parentNode->subdivided & !subdivided & inDrawDistance) {
		subdivide();
		c00->updateWithoutSubdivision(camera);
		c01->updateWithoutSubdivision(camera);
		c10->updateWithoutSubdivision(camera);
		c11->updateWithoutSubdivision(camera);
	}
}
void QuadtreeNodeRenderData::render(ShaderSystem& shaderSystem)
{
	shaderSystem.uniforms.setVec3("chunkOffsetVector", chunkOffset);
	shaderSystem.uniforms.setVec4("worldOffset", Vec4(parentNode->id.location.x, parentNode->id.location.y, parentNode->data.terrain.lowerLimit, parentNode->id.level));
	shaderSystem.currentShaderProgram().updateUniforms();
	if (terrainMesh.loaded)terrainMesh.render();
}
void QuadtreeNodeRenderData::updateWithoutSubdivision(PlanetCamera& camera)
{
	//create 3d chunk id
	OctreeID tmp;
	tmp.level = parentNode->id.level;
	tmp.location.x = parentNode->id.location.x;
	tmp.location.y = parentNode->id.location.y;
	tmp.location.z = parentNode->data.terrain.lowerLimit;

	//get camera relative location
	chunkOffset = cameraRelativeLocationOfChunk(tmp, camera);

	inDrawDistance = glm::length(chunkOffset + Vec3(0.5,0.5,0)) < 3.0f;

	if (!inDrawDistance) unsubdivide();

	if (subdivided) {
		c00->update(camera);
		c01->update(camera);
		c10->update(camera);
		c11->update(camera);
	}
}
void QuadtreeNodeRenderData::create(SharedPointer<QuadtreeNode> quadtreeNode)
{
	parentNode = quadtreeNode;

	CPUMesh cpuTerrainMesh = createTerrainMesh(parentNode->data.terrain.heightmapForNormals);
	terrainMesh.upload(cpuTerrainMesh);

	parentNode->incrementUsers();
}
void QuadtreeNodeRenderData::renderLevel(UShort level, ShaderSystem& shaderSystem)
{
	if (subdivided) {
		c00->renderLevel(level, shaderSystem);
		c01->renderLevel(level, shaderSystem);
		c10->renderLevel(level, shaderSystem);
		c11->renderLevel(level, shaderSystem);
	}
	else {
		if (parentNode->id.level == level) {
			render(shaderSystem);
		}
	}
}
