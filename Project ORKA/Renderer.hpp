#pragma once

#include "GameSimulation.hpp"
#include "Framebuffer.hpp"
#include "RenderObjectSystem.hpp"
#include "ViewportSystem.hpp"
#include "Math.hpp"
#include "UserInterface.hpp"
#include "Debug.hpp"

#define TARGETFRAMERATE 60

struct Renderer {
	Bool chunkBorders = false;
	Bool wireframeMode = false;
	Bool worldDistortion = true;
	Bool adjustRenderVariables = true;
	Bool pauseWorldDataCollection = false;
	Short multisampleCount = 4;

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

	//synchronization
	Mutex mutex;
	void sync();
	
	//(de)initialization
	void create();
	void destroy();

	//main render function
	void render();

	//RENDERING
	//generic framebuffer operations
	void clearDepth();
	void clearColor(Color color);
	void updateUniforms();
	void resetModelMatrix();


	//render functions
		//worldSystem
	void renderWorldChunk(WorldChunk & chunk);
	void renderPlanetSystem(PlanetSystem& planetSystem);
	void renderFramebufferInQuad();
	
	Camera& currentCamera();
	Uniforms& uniforms();
	Viewport& currentViewport();
	Framebuffer& currentFramebuffer();
};

void culling(bool isCulling);
void depthTest(bool isUsingDepth);
void updateWireframeMode(bool wireframeMode);
void dynamicallyAdjustValue(Renderer& renderer, Float& value);

void renderSpaceShip(Renderer & renderer, SpaceShip & spaceShip);

void pollGraphicsAPIError();