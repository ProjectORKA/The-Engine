
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
#include "VoxelSystem.hpp"

struct Player;
struct Camera;

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
	VoxelRenderer voxelRenderer;
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

	//spaces
	void screenSpace();
	void normalizedSpace();
	void apectCorrectNormalizedSpace();
	
	//meshes
	void renderMesh(Name name);

	//textures
	void useTexture(Name name);
	void useTexture(Name name, Index location);

	//shaders
	Index useShader(Name name);

	//colors
	void fill(Vec4 color);
	void clearColor(Color color);

	//primitives
	void line(Vec3 start, Vec3 end, Float width);
	void arrow(Vec3 start, Vec3 end);

	//text
	void renderText(String text, Vec2 position, FontStyle font);

	//sky
	void renderSky(Camera& camera);
	void renderAtmosphere(Player& player, Vec3 sunDirection);

	Bool getCulling();
	void setWireframeMode();
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