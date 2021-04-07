#pragma once

#include "Settings.hpp"
#include "Time.hpp"
//sub systems
#include "CameraSystem.hpp"
#include "ViewportSystem.hpp"
#include "TextRenderSystem.hpp"
#include "FramebufferSystem.hpp"
#include "MeshSystem.hpp"
#include "ShaderSystem.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"

struct Renderer{
	Time renderTime;

	//settings
	Bool wireframeMode = false;
	Bool adjustRenderVariables = true;
	Short multisampleCount = 0;

	//framerate
	Float targetFrameRate = TARGETFRAMERATE;

	//render data
	CameraSystem cameraSystem;
	ViewportSystem viewportSystem;
	//TextRenderSystem textRenderSystem;
	FramebufferSystem framebufferSystem;
	MeshSystem meshSystem;
	TextureSystem textureSystem;
	ShaderSystem shaderSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	Mutex mutex;
	void waitForFinishedFrame();

	void create();
	void destroy();

	void begin();
	void end();

	void clearDepth();
	void clearColor();
	void renderFramebuffer();
	void clearColor(Color color);

	void setCulling(Bool isCulling);
	void setDepthClamp(Bool depthClamp);
	void setDepthTest(Bool isUsingDepth);
	void setAlphaBlending(Bool blending);
	void setWireframeMode(Bool isWireframeMode);

	void updateUniforms();
	void resetModelMatrix();
	
	void pollGraphicsAPIError();
	
	Float& aspectRatio();
	Uniforms& uniforms();
	Viewport& currentViewport();
	Framebuffer& currentFramebuffer();
};