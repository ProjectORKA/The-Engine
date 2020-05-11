
#pragma once

//struct MeshRenderInfo {
//	//render essentials
//	GPUMesh* gpuMesh;
//
//	//pipeline children
//	Vector<Matrix> modelMatrices;
//};
//struct ShaderProgramRenderInfo {
//	//render essentials
//	Index shaderID;
//
//	//pipeline children
//	Vector<MeshRenderInfo> meshRenderInfo;
//};
//struct RenderObjectRenderInfo {
//	Index nameID;
//	Index meshID;
//};
//struct WorldChunkRenderInfo {
//	//render essentials
//	Vec3 chunkOffsetVector;
//	Vec4 worldOffsetVector;
//	//pipeline children
//	Vector<ShaderProgramRenderInfo> shaderProgramRenderInfo;
//};
//struct WorldLevelRenderInfo {
//	//pipeline children
//	Vector<WorldChunkRenderInfo> worldChunkRenderInfo;
//};
//struct ViewportRenderInfo {
//	//render essentials
//	Int absoluteWidth = 0;
//	Int absoluteHeight = 0;
//	Int xPos = 0;
//	Int yPos = 0;
//	Matrix projectionMatrix = Matrix(1);
//	Matrix vpMatrix = Matrix(1);
//};
//struct CameraRenderInfo {
//	//render essentials
//	Matrix viewMatrix = Matrix(1);
//
//	//pipeline children
//	Vector<ViewportRenderInfo> viewportRenderInfo;
//	Vector<WorldLevelRenderInfo> worldLevelRenderInfo;
//};
//struct FramebufferRenderInfo {
//	Color color;
//	Vector<CameraRenderInfo> cameraRenderInfo;
//};
//struct Pipeline {
//	Bool backfaceCulling = true;
//
//	Float totalTime = 0.0f;
//
//	FramebufferRenderInfo framebufferRenderInfo;
//};