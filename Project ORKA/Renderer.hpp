
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
#include "LineRenderer.hpp"
#include "InteractionSystem.hpp"
#include "RectangleRenderer.hpp"
#include "IdFramebuffer.hpp"

struct Player;
struct Camera;

struct Renderer{
	Time time;
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
	LineRenderer lineRenderer;
	TextureSystem textureSystem;
	VoxelRenderer voxelRenderer;
	IdFramebuffer idFramebuffer;
	TextRenderSystem textRenderSystem;
	RectangleRenderer rectangleRenderer;
	FramebufferSystem framebufferSystem;
	InteractionSystem interactionSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	Mutex mutex;
	void sync(); //makes non renderer threads wait for the finished frame

	void create(Area size);
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
	void aspectCorrectNormalizedSpace();
	void aspectRatioNormalizedSpace(Float aspectRatio) {
		Area renderSize = getArea();
		Float renderAspectRatio = getAspectRatio();
			uniforms().vMatrix() = Matrix(1);
			//scale the view up based on the target aspect ratio
			Matrix pMatrix(1);
			Float scaleUpFactor = max(aspectRatio, 1 / aspectRatio);
			pMatrix = scale(pMatrix, Vec3(scaleUpFactor, scaleUpFactor, 1));


			if (renderAspectRatio > 1) uniforms().pMatrix() = scale(pMatrix, Vec3(1 / renderAspectRatio, 1, 1));
			else uniforms().pMatrix() = scale(pMatrix, Vec3(1, renderAspectRatio, 1));
	}
	
	//meshes
	void rerenderMesh();
	void useMesh(Name name);
	void renderMesh(Name name);
	void renderMesh(Index meshID);
	void renderMeshInstanced(Name name);

	//textures
	void useTexture(Name name);
	void useTexture(Name name, Index location);

	//shaders
	Index useShader(Name name);

	//colors
	void fill(Vec4 color);
	void clearColor(Color color);

	//primitives
	void arrow(Vec3 start, Vec3 end);
	void circle(Vec2 pos, Float radius);
	void rectangle(Vec2 pos, Vec2 size);
	void line(Vec3 start, Vec3 end, Float width);
	void line(Vec2 start, Vec2 end, Float width);

	//text
	void renderText(String text, Vec2 position, FontStyle font);

	//sky
	void renderSky(Camera& camera);
	void renderAtmosphere(Player& player, Vec3 sunDirection);

	void setWireframeMode();
	void setColor(Color color);
	void pollGraphicsAPIError();
	void setCulling(Bool isCulling);
	void setDepthClamp(Bool depthClamp);
	void setDepthTest(Bool isUsingDepth);
	void setAlphaBlending(Bool blending);
	void setWireframeMode(Bool isWireframeMode);
	void createBlurTexture(Index from, Index to);
	void addRenderObject(RenderObjectNames renderObjectNames);
	
	Area getArea();
	
	Bool getCulling();

	Float deltaTime();
	Float getAspectRatio();

	Uniforms& uniforms();
};