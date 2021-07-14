#pragma once

#include "Settings.hpp"
#include "Time.hpp"
//sub systems
#include "CameraSystem.hpp"
#include "TextRenderSystem.hpp"
#include "FramebufferSystem.hpp"
#include "MeshSystem.hpp"
#include "ShaderSystem.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"
#include "RenderRegion.hpp"

struct Renderer{
	Time renderTime;
	//settings
	Bool wireframeMode = false;
	Bool adjustRenderVariables = true;

	//framerate
	Float targetFrameRate = TARGETFRAMERATE;
	ULL frameCount = 0;

	//render data
	CameraSystem cameraSystem;
	RenderRegion renderRegion;
	
	TextRenderSystem textRenderSystem;
	FramebufferSystem framebufferSystem;
	MeshSystem meshSystem;
	TextureSystem textureSystem;
	ShaderSystem shaderSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;


	Mutex mutex;
	void sync(); //makes non renderer threads wait for the finished frame

	void create();
	void destroy();

	void begin();
	void end();

	void clearDepth();
	void clearColor();
	void clearColor(Color color);

	void pollGraphicsAPIError();
	void setCulling(Bool isCulling);
	void setDepthClamp(Bool depthClamp);
	void setDepthTest(Bool isUsingDepth);
	void setAlphaBlending(Bool blending);
	void setWireframeMode(Bool isWireframeMode);
	void addRenderObject(RenderObjectNames renderObjectNames);
	void createBlurTexture(Framebuffer& from, Framebuffer& to);
	
	Float& aspectRatio();
	Uniforms& uniforms();
};

void loadPrimitives(Renderer& renderer);