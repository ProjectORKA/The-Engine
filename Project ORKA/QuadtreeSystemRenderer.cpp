
#include "QuadtreeSystemRenderer.hpp"
#include "Renderer.hpp"

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
	if (subdivided) {
		unsubdivide();
	}

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
void QuadtreeNodeRenderData::update(PlanetCamera& camera)
{
	updateWithoutSubdivision(camera);

	if (equivalentQuadtreeNode->subdivided & !subdivided & inDrawDistance) {
		subdivide();
		c00->updateWithoutSubdivision(camera);
		c01->updateWithoutSubdivision(camera);
		c10->updateWithoutSubdivision(camera);
		c11->updateWithoutSubdivision(camera);
	}
}
void QuadtreeNodeRenderData::create(QuadtreeNode & quadtreeNode)
{
	equivalentQuadtreeNode = &quadtreeNode;

	CPUMesh cpuMesh = createTerrainMesh(equivalentQuadtreeNode->data.terrain.heightmapForNormals);

	terrainMesh.upload(cpuMesh);

	equivalentQuadtreeNode->incrementUsers();
}
void QuadtreeNodeRenderData::updateWithoutSubdivision(PlanetCamera& camera)
{
	//create 3d chunk id
	OctreeID tmp;
	tmp.level = equivalentQuadtreeNode->id.level;
	tmp.location.x = equivalentQuadtreeNode->id.location.x;
	tmp.location.y = equivalentQuadtreeNode->id.location.y;
	tmp.location.z = equivalentQuadtreeNode->data.terrain.lowerLimit;

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
void QuadtreeNodeRenderData::render(ShaderSystem& shaderSystem, MeshSystem & meshSystem)
{
	//camera culling
	//Vec4 chunkCenterOnScreen = shaderSystem.uniforms.data.vpMatrix * (shaderSystem.uniforms.data.chunkOffsetVector + Vec4(0.5, 0.5, 0.5, 1));

	/*	if (
		(chunkCenterOnScreen.x > -1 & chunkCenterOnScreen.x < 1) &
		(chunkCenterOnScreen.y > -1 & chunkCenterOnScreen.y < 1) &
		(chunkCenterOnScreen.z > -1 & chunkCenterOnScreen.z < 1)
		) */
	{
		shaderSystem.uniforms.data.chunkOffsetVector = Vec4(chunkOffset,1);
		shaderSystem.uniforms.data.worldOffset = Vec4(equivalentQuadtreeNode->id.location.x, equivalentQuadtreeNode->id.location.y, equivalentQuadtreeNode->data.terrain.lowerLimit, equivalentQuadtreeNode->id.level);
		shaderSystem.uniforms.update();
	
		terrainMesh.render();
	}
}
void QuadtreeNodeRenderData::renderLevel(UShort level, ShaderSystem& shaderSystem, MeshSystem & meshsystem)
{
	if (subdivided) {
		c00->renderLevel(level, shaderSystem, meshsystem);
		c01->renderLevel(level, shaderSystem, meshsystem);
		c10->renderLevel(level, shaderSystem, meshsystem);
		c11->renderLevel(level, shaderSystem, meshsystem);
	}
	else {
		if (equivalentQuadtreeNode->id.level == level) {
			render(shaderSystem, meshsystem);
		}
	}
}

void QuadtreeRenderSystem::create(Renderer& renderer){
	grassTextureID = renderer.textureSystem.getTextureID("grass");
	terrainShaderID = renderer.shaderSystem.getShaderID("terrain");
}
void QuadtreeRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera& planetCamera) {
	
	if (root.equivalentQuadtreeNode == nullptr) root.create(planetSystem.quadtreeSystem.root);
	root.update(planetCamera);

	planetCamera.render(renderer.uniforms(), renderer.renderRegion.getAspectRatio());
	renderer.shaderSystem.use(terrainShaderID);
	renderer.textureSystem.use(grassTextureID);
	renderer.shaderSystem.uniforms.data.distortion = Int(worldDistortion);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.uniforms.update();

	renderer.setDepthTest(true);
	renderer.setCulling(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {
		glClear(GL_DEPTH_BUFFER_BIT);
		root.renderLevel(level, renderer.shaderSystem, renderer.meshSystem);
	}
}
void QuadtreeRenderSystem::count()
{
	root.count();
}
void QuadtreeRenderSystem::destroy() {
	root.destroy();
}
