#pragma once

#include "Settings.hpp"
#include "Time.hpp"
//sub systems
#include "TextRenderSystem.hpp"
#include "FramebufferSystem.hpp"
#include "MeshSystem.hpp"
#include "MatrixSystem.hpp"
#include "ShaderSystem.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"
#include "RenderRegion.hpp"
#include "SDFTerrainRenderSystem.hpp"

struct Renderer{
	Time renderTime;
	//settings
	Bool wireframeMode = false;
	Bool adjustRenderVariables = true;

	//framerate
	Float targetFrameRate = TARGETFRAMERATE;
	ULL frameCount = 0;

	RenderRegion renderRegion;

	//render systems
	MeshSystem meshSystem;
	ShaderSystem shaderSystem;
	MatrixSystem matrixSystem;
	TextureSystem textureSystem;
	TextRenderSystem textRenderSystem;
	FramebufferSystem framebufferSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;
	SDFTerrainRenderSystem sdfTerrainRenderSystem;

	Mutex mutex;
	void sync(); //makes non renderer threads wait for the finished frame

	void create();
	void destroy();

	//drawing a frame
	void begin();
	void end();

	//clear
	void clearDepth();
	void clearColor();
	void clearColor(Color color);

	//spaces
	void screenSpace();
	void normalizedSpace();
	
	//meshes
	void renderMesh(Name name);

	//textures
	void useTexture(Name name) {
		textureSystem.use(name);
	}
	void useTexture(Name name, Index location) {
		textureSystem.use(name, location);
	}

	//shaders
	void useShader(Name name) {
		shaderSystem.use(name);
	}

	void pollGraphicsAPIError();
	void setCulling(Bool isCulling);
	void setDepthClamp(Bool depthClamp);
	void setDepthTest(Bool isUsingDepth);
	void setAlphaBlending(Bool blending);
	void setWireframeMode(Bool isWireframeMode);
	void createBlurTexture(Index from, Index to);
	void addRenderObject(RenderObjectNames renderObjectNames);
	
	Float& aspectRatio();
	Uniforms& uniforms();
};

void loadPrimitives(Renderer& renderer);