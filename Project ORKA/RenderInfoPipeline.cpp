
#include "GameSimulation.hpp"
#include "Camera.hpp"

//void buildMeshRenderInfo(Renderer& renderer, ShaderProgramRenderInfo& shaderProgramRenderInfo, Index meshID, Matrix modelMatrix) {
//
//	GPUMesh& gpuMesh = renderer.meshSystem.gpuMeshes[meshID];
//	if (gpuMesh.readyToRender) {
//		shaderProgramRenderInfo.meshRenderInfo.emplace_back();
//		shaderProgramRenderInfo.meshRenderInfo.back().gpuMesh = &gpuMesh;
//		shaderProgramRenderInfo.meshRenderInfo.back().modelMatrices.push_back(modelMatrix);
//	}
//}
//
//void buildShaderRenderInfo(Renderer& renderer, WorldChunkRenderInfo& worldChunkRenderInfo, std::shared_ptr<Chunk>& chunkPtr) {
//	//for (EntityTypeContainer& container : chunkPtr->data.entityData) {
//
//	//	auto it = container.type.find(RenderComponentType);
//
//	//	if (it != container.type.end()) {
//	//		it.
//	//	}
//
//	//	Index renderObjectID = renderer.renderObjectSystem.renderComponentIDToRenderObjectID[renderer.gameSimulation->entitySystem.entityTypes.[entityID].indices[RenderComponentType]];
//	//	RenderObject& renderObject = renderer.renderObjectSystem.renderObjects[renderObjectID];
//
//	//	//shader id
//	//	worldChunkRenderInfo.shaderProgramRenderInfo.emplace_back();
//	//	worldChunkRenderInfo.shaderProgramRenderInfo.back().shaderProgram = &renderer.shaderSystem.shaderPrograms[renderObject.shaderProgramID];
//	//	renderer.gameSimulation->modelMatrixSystem.mutex.lock_shared();
//	//	buildMeshRenderInfo(renderer, worldChunkRenderInfo.shaderProgramRenderInfo.back(), renderObject.meshID, renderer.gameSimulation->modelMatrixSystem.modelMatrices[renderer.gameSimulation->entitySystem.entities[entityID].indices[ModelMatrixComponentType]]);
//	//	renderer.gameSimulation->modelMatrixSystem.mutex.unlock_shared();
//	//}
//}
//
//void buildWorldRenderInfo(Renderer& renderer, OctreeWorldSystemCamera& camera, CameraRenderInfo& cameraRenderInfo, std::shared_ptr<Chunk>& chunkPtr, bool& canBeRendered) {
//
//	Chunk& chunk = *chunkPtr;
//	chunk.mutex.lock_shared();
//
//	//get location of chunk
//	//unsigned short levelBasedBitShift = 64 - chunk.level;
//	unsigned long long offset = ULLONG_MAX >> (chunk.level + 1);
//
//	long long x = chunk.location.x << 64 - chunk.level;	//converts to actual position
//	long long y = chunk.location.y << 64 - chunk.level;	//converts to actual position
//	long long z = chunk.location.z << 64 - chunk.level; //converts to actual position
//
//	//subtract location of camera to get delta/distance
//	x -= camera.chunkLocation.x - offset;
//	y -= camera.chunkLocation.y - offset;
//	z -= camera.chunkLocation.z - offset;
//
//	//turn integer to float
//	glm::vec3 delta(x, y, z);
//
//	//imitate scale by moving it closer/further instead of scaling (allows for constant clipping)
//	delta /= pow(2, 64 - chunk.level);
//
//	//check visibility
//	bool render = true;
//	bool hasRenderableContent = true; //chunk.entityIDs.size() > 0;
//	bool isLowestLevel = chunk.level >= CHUNK_LEVEL_MAX;
//	bool isInRenderDistance = glm::length(delta) < glm::pow(CHUNK_DISTANCE_MULTIPLIER, renderer.settings.worldSystemRenderDistance);
//
//	if (isInRenderDistance & hasRenderableContent) {
//		setChunkIsInUse(chunk, *renderer.gameSimulation);
//		if (chunk.subdivided) {
//			bool canChildChunksRender = true;
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.tfr, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.tfl, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.tbr, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.tbl, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.bfr, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.bfl, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.bbr, canChildChunksRender);
//			buildWorldRenderInfo(renderer, camera, cameraRenderInfo, chunk.bbl, canChildChunksRender);
//			if (canChildChunksRender) {
//				render = false;
//			}
//			else {
//				render = true;
//			}
//		}
//		else {
//			render = true;
//		}
//	}
//	else {
//		canBeRendered = false;
//		render = false;
//	}
//
//	if (render) {
//		delta -= (camera.location / (float)pow(2, 64 - chunk.level));
//		while (cameraRenderInfo.worldLevelRenderInfo.size() < chunk.level + 1) cameraRenderInfo.worldLevelRenderInfo.emplace_back();
//
//		cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.emplace_back();
//		cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.back().chunkOffsetVector = delta;
//		cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.back().worldOffsetVector = Vec4(chunk.location, chunk.level);
//		buildShaderRenderInfo(renderer, cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.back(), chunkPtr);
//		if (cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.back().shaderProgramRenderInfo.size() == 0) {
//			cameraRenderInfo.worldLevelRenderInfo[chunk.level].worldChunkRenderInfo.pop_back();
//		}
//	}
//
//	chunk.mutex.unlock_shared();
//}
//
//void buildViewPortRenderInfo(CameraRenderInfo& cameraRenderInfo, Viewport& viewport, Camera& camera, Framebuffer& framebuffer) {
//	cameraRenderInfo.viewportRenderInfo.emplace_back();
//
//	cameraRenderInfo.viewportRenderInfo.back().absoluteWidth = viewport.relativeWidth * framebuffer.width;
//	cameraRenderInfo.viewportRenderInfo.back().absoluteHeight = viewport.relativeHeight * framebuffer.height;
//	cameraRenderInfo.viewportRenderInfo.back().xPos = viewport.relativeX * framebuffer.width;
//	cameraRenderInfo.viewportRenderInfo.back().yPos = viewport.relativeY * framebuffer.height;
//
//	cameraRenderInfo.viewportRenderInfo.back().projectionMatrix =
//		glm::perspective(
//			glm::radians(camera.fieldOfView),
//			(float)cameraRenderInfo.viewportRenderInfo.back().absoluteWidth /
//		(float)cameraRenderInfo.viewportRenderInfo.back().absoluteHeight,
//			camera.nearClipValue,
//			camera.farClipValue);
//
//	cameraRenderInfo.viewportRenderInfo.back().vpMatrix = cameraRenderInfo.viewportRenderInfo.back().projectionMatrix * cameraRenderInfo.viewMatrix;
//}
//
//void buildCameraRenderInfo(Renderer& renderer, FramebufferRenderInfo& framebufferRenderInfo, OctreeWorldSystemCamera& camera, Framebuffer& framebuffer) {
//	framebufferRenderInfo.cameraRenderInfo.emplace_back();
//	framebufferRenderInfo.cameraRenderInfo.back().viewMatrix = glm::lookAt(Vec3(0), Vec3(0) + camera.forwardVector, camera.upVector);
//	framebufferRenderInfo.cameraRenderInfo.back().worldLevelRenderInfo.clear();
//
//	framebufferRenderInfo.cameraRenderInfo.back().viewportRenderInfo.clear();
//	for (Viewport* viewport : camera.viewportPointers) {
//		buildViewPortRenderInfo(framebufferRenderInfo.cameraRenderInfo.back(), *viewport, camera, framebuffer);
//	}
//	bool placeholder;
//	buildWorldRenderInfo(renderer, camera, framebufferRenderInfo.cameraRenderInfo.back(), renderer.gameSimulation->worldSystem.octreeRoot, placeholder);
//}
//
//void buildFramebufferInfo(Renderer& renderer, FramebufferRenderInfo& framebufferRenderInfo, Framebuffer& framebuffer) {
//
//	framebufferRenderInfo.color = framebuffer.backgroundColor;
//
//	framebufferRenderInfo.cameraRenderInfo.clear();
//	for (OctreeWorldSystemCamera* camera : framebuffer.cameraPointers) {
//		buildCameraRenderInfo(renderer, framebufferRenderInfo, *camera, framebuffer);
//	}
//}
//
//void buildRenderPipeline(Renderer& renderer) {
//	//create the data
//	buildFramebufferInfo(renderer, renderer.pipeline.framebufferRenderInfo, renderer.framebuffer);
//}
//
////render pipeline
//void renderMeshRI(GPUMesh& gpuMesh, Vector<Matrix>& modelMatrices, ShaderSystem & shaderSystem, Index & shaderID) {
//	bindMesh(gpuMesh);
//	for (Matrix mMatrix : modelMatrices) {
//		shaderSystem.uniforms.updateUniform("mMatrix", mMatrix);
//		useProgramAndUpdateUniforms(shaderSystem, shaderID);
//		renderMesh(gpuMesh);
//	}
//}
//
//void renderShaderProgramRI(Renderer& renderer, ShaderProgramRenderInfo& shaderProgramRenderInfo, CameraRenderInfo& cameraRenderInfo, WorldChunkRenderInfo& worldChunkRenderInfo) {
//	for (MeshRenderInfo& meshRenderInfo : shaderProgramRenderInfo.meshRenderInfo) {
//		renderMeshRI(*meshRenderInfo.gpuMesh, meshRenderInfo.modelMatrices, renderer.shaderSystem, shaderProgramRenderInfo.shaderID);
//	}
//}
//
//void renderWorldChunkRI(Renderer& renderer, WorldChunkRenderInfo& worldChunkRenderInfo, CameraRenderInfo& cameraRenderInfo, ViewportRenderInfo& viewportRenderInfo) {
//
//	renderer.shaderSystem.uniforms.updateUniform("chunkOffset", worldChunkRenderInfo.chunkOffsetVector);
//	renderer.shaderSystem.uniforms.updateUniform("worldOffset", worldChunkRenderInfo.worldOffsetVector);
//
//	for (ShaderProgramRenderInfo& shaderProgramRenderInfo : worldChunkRenderInfo.shaderProgramRenderInfo) {
//		renderShaderProgramRI(renderer, shaderProgramRenderInfo, cameraRenderInfo, worldChunkRenderInfo);
//	}
//}
//
//void renderWorldLevelRI(Renderer& renderer, WorldLevelRenderInfo& worldLevelRenderInfo, CameraRenderInfo& cameraRenderInfo, ViewportRenderInfo& viewportRenderInfo) {
//
//	//proceed to child nodes
//	for (WorldChunkRenderInfo& worldChunkRenderInfo : worldLevelRenderInfo.worldChunkRenderInfo) {
//		renderWorldChunkRI(renderer, worldChunkRenderInfo, cameraRenderInfo, viewportRenderInfo);
//	}
//
//	glClear(GL_DEPTH_BUFFER_BIT);
//}
//
//void renderTreeViewportRI(Renderer& renderer, CameraRenderInfo& cameraRenderInfo, ViewportRenderInfo& viewportRenderInfo) {
//
//	//check if viewport is big enough
//	if (!viewportRenderInfo.absoluteWidth | !viewportRenderInfo.absoluteHeight) return;
//
//	//set viewport size
//	glViewport(viewportRenderInfo.xPos, viewportRenderInfo.xPos, viewportRenderInfo.absoluteWidth, viewportRenderInfo.absoluteHeight);
//
//	renderer.shaderSystem.uniforms.updateUniform("vpMatrix", viewportRenderInfo.vpMatrix);
//
//	for (WorldLevelRenderInfo& worldLevelRenderInfo : cameraRenderInfo.worldLevelRenderInfo) {
//		renderWorldLevelRI(renderer, worldLevelRenderInfo, cameraRenderInfo, viewportRenderInfo);
//	}
//}
//
//void renderCameraRI(Renderer& renderer, CameraRenderInfo& cameraRenderInfo) {
//	//carry data and proceed to child nodes
//	for (ViewportRenderInfo& viewportRenderInfo : cameraRenderInfo.viewportRenderInfo) {
//		renderTreeViewportRI(renderer, cameraRenderInfo, viewportRenderInfo);
//	}
//}
//
//void renderFramebufferRI(Renderer& renderer, FramebufferRenderInfo& framebufferRenderInfo) {
//
//	//background
//	renderBackground(framebufferRenderInfo.color);
//
//	//proceed to child nodes
//	for (CameraRenderInfo& cameraRenderInfo : framebufferRenderInfo.cameraRenderInfo) {
//		renderCameraRI(renderer, cameraRenderInfo);
//	}
//}
//
//void renderPipeline(Renderer& renderer, Pipeline& pipeline) {
//
//	depthTest(true);
//
//	culling(pipeline.backfaceCulling);
//
//
//	//wireframe
//	updateWireframeMode(pipeline.wireframeMode);
//
//	renderer.shaderSystem.uniforms.updateUniform("time", renderer.renderTime.total);
//
//	renderFramebufferRI(renderer, pipeline.framebufferRenderInfo);
//}