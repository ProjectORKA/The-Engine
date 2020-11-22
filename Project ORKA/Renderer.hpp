#pragma once

#include "GameSimulation.hpp"
#include "Framebuffer.hpp"
#include "RenderObjectSystem.hpp"
#include "ViewportSystem.hpp"
#include "Math.hpp"
#include "UserInterface.hpp"
#include "Debug.hpp"
#include "PlanetRenderSystem.hpp"

#define TARGETFRAMERATE 60

struct Renderer {
	Bool wireframeMode = false;
	Bool adjustRenderVariables = true;
	//Bool pauseWorldDataCollection = false;
	Short multisampleCount = 0;

	Time renderTime;
	GameSimulation* gameSimulation = nullptr;
	//UserInterface ui;

	//framerate
	Float targetFrameRate = 75;

	//render data
	FramebufferSystem framebufferSystem;
	ViewportSystem viewportSystem;
	CameraSystem cameraSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

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