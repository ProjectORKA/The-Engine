#pragma once

#include "GameSimulation.hpp"
#include "Framebuffer.hpp"
#include "RenderObjectSystem.hpp"
#include "ViewportSystem.hpp"


#define TARGETFRAMERATE 75
#define DYNAMIC_VIEW_DISTANCE_MULTIPLIER 1.001f	//controls the increase in distance for the world system

struct RenderSettings {
	//settings
	Bool chunkBorders = false;
	Bool adjustRenderVariables = true;

	UInt worldSystemRenderDistance = 100;

	Float minimumFrameRate = TARGETFRAMERATE;
	Float maximumFrameRate = minimumFrameRate * 1.2;
};
struct Renderer {
	GameSimulation* gameSimulation = nullptr;
	Time renderTime;
	RenderSettings settings;

	Bool wireframeMode = false;

	//render source
	Framebuffer framebuffer;
	ViewportSystem viewportSystem;
	CameraSystem cameraSystem;

	//render state
	RenderObjectSystem renderObjectSystem;

	Bool renderSync = false;
	void sync();

	void create();
	void render();
	void renderTest();
	void destroy();

	void clearDepth();
	void clearColor(Color color);
	void updateUniforms();
};

void culling(bool isCulling);
void depthTest(bool isUsingDepth);
void updateWireframeMode(bool wireframeMode);
void dynamicallyAdjustValue(Renderer& renderer, unsigned int& value);