#pragma once

#include "GameSimulation.hpp"
#include "Framebuffer.hpp"
#include "RenderObjectSystem.hpp"
#include "ViewportSystem.hpp"
#include "Math.hpp"
#include "UserInterface.hpp"
#include "Debug.hpp"

#define TARGETFRAMERATE 60

struct WorldRenderChunk {
	WorldChunk* chunk;
	Vec3 chunkOffsetVector;
};

using WorldRenderData = Vector<WorldRenderChunk>[64];

struct Renderer {
	Bool chunkBorders = false;
	Bool wireframeMode = false;
	Bool worldDistortion = true;
	Bool adjustRenderVariables = true;
	Bool pauseWorldDataCollection = false;

	Time renderTime;
	GameSimulation* gameSimulation = nullptr;
	//UserInterface ui;

	//framerate
	Float targetFrameRate = 100;

	//render data
	FramebufferSystem framebufferSystem;
	ViewportSystem viewportSystem;
	CameraSystem cameraSystem;
	RenderObjectSystem renderObjectSystem;

	//worldSystem
	Float worldSystemRenderDistance = 5;
	WorldRenderData worldRenderData;
	void createWorldRenderData(WorldChunk & world);
	void renderWorld(WorldRenderData& world);

	Mutex mutex;
	void sync();

	void create();
	void destroy();

	void render();
	void renderTest();

	void clearDepth();
	void clearColor(Color color);

	void updateUniforms();
	void resetModelMatrix();
	Uniforms& uniforms();

	Viewport& currentViewport();
};

void culling(bool isCulling);
void depthTest(bool isUsingDepth);
void updateWireframeMode(bool wireframeMode);
void dynamicallyAdjustValue(Renderer& renderer, Float& value);

void renderSpaceShip(Renderer & renderer, SpaceShip & spaceShip);

void pollGraphicsAPIError();