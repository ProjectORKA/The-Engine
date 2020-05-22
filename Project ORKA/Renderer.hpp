#pragma once

#include "GameSimulation.hpp"
#include "Framebuffer.hpp"
#include "RenderObjectSystem.hpp"
#include "ViewportSystem.hpp"
#include "HeightMap.hpp"

#define TARGETFRAMERATE 60

struct RenderSettings {
	//settings
	Bool chunkBorders = false;
	Bool adjustRenderVariables = true;

	UInt worldSystemRenderDistance = 10;

	Float minimumFrameRate = TARGETFRAMERATE;
	Float maximumFrameRate = minimumFrameRate * 1.2;
};

struct WorldRenderChunk {
	Chunk* chunk;
	Vec3 chunkOffsetVector;
};

using WorldRenderData = Vector<WorldRenderChunk>[64];

struct Renderer {
	GameSimulation* gameSimulation = nullptr;
	Time renderTime;
	RenderSettings settings;

	Bool wireframeMode = false;
	Bool worldDistortion = false;

	//render source
	Framebuffer framebuffer;
	ViewportSystem viewportSystem;
	CameraSystem cameraSystem;

	//render state
	RenderObjectSystem renderObjectSystem;
	WorldRenderData worldRenderData;

	Mutex mutex;

	void sync();

	void create();
	void render();
	void renderTest();
	void destroy();

	void resetModelMatrix();
	void clearDepth();
	void clearColor(Color color);
	void updateUniforms();


	void createWorldRenderData(Chunk& world);
	void renderWorld(WorldRenderData& world);
};

void culling(bool isCulling);
void depthTest(bool isUsingDepth);
void updateWireframeMode(bool wireframeMode);
void dynamicallyAdjustValue(Renderer& renderer, unsigned int& value);