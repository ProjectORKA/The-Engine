
#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Settings.hpp"
#include "RenderRegion.hpp"
#include "MeshSystem.hpp"
#include "ShaderSystem.hpp"
#include "MatrixSystem.hpp"
#include "LineRenderer.hpp"
#include "TextureSystem.hpp"
#include "IdFramebuffer.hpp"
#include "TextRenderSystem.hpp"
#include "RectangleRenderer.hpp"
#include "FramebufferSystem.hpp"
#include "InteractionSystem.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"

struct Camera;
struct Player;


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
	IdFramebuffer idFramebuffer;
	TextRenderSystem textRenderSystem;
	RectangleRenderer rectangleRenderer;
	FramebufferSystem framebufferSystem;
	InteractionSystem interactionSystem;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	Mutex mutex;
	void sync(); //makes non renderer threads wait for the finished frame

	void create(Engine& engine, Area size);
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
	void normalizedScreenSpace();
	void aspectCorrectNormalizedSpace();
	void normalizedSpaceWithAspectRatio(Float aspectRatio);
	
	//meshes
	void rerenderMesh();
	void renderMesh(Index meshID);
	void useMesh(Engine& engine, Name name);
	void renderMesh(Engine& engine, Name name);
	void renderMeshInstanced(Engine& engine, Name name);

	//textures
	void useTexture(Engine& engine, Name name);
	void useTexture(Engine& engine, Name name, Index location);

	//shaders
	Index useShader(Engine& engine, Name name);
	void postProcess(Engine& engine, Name name);
	void fullScreenShader(Engine& engine, Name name);

	//framebuffers
	void read(Name name);
	void draw(Name name);

	//colors
	void fill(Vec3 color);
	void fill(Vec4 color);
	void clearColor(Color color);
	void fill(Float r, Float g, Float b);

	//primitives
	void line(Vec3 start, Vec3 end, Float width);
	void line(Vec2 start, Vec2 end, Float width);
	void arrow(Engine& engine, Vec2 start, Vec2 end);
	void arrow(Engine& engine, Vec3 start, Vec3 end);
	void rectangle(Engine& engine, Vec2 pos, Vec2 size);
	void circle(Engine& engine, Vec2 pos, Float radius);

	//text
	void renderText(Engine& engine, String text, Vec2 position, FontStyle font);

	//sky
	void renderSky(Engine& engine, Camera& camera);
	void renderAtmosphere(Engine& engine, Player& player, Vec3 sunDirection);

	//blending
	void blendModeAdditive();
	void setAlphaBlending(Bool blending);

	void setWireframeMode();
	void setColor(Color color);
	void pollGraphicsAPIError();
	void setCulling(Bool isCulling);
	void setDepthClamp(Bool depthClamp);
	void setDepthTest(Bool isUsingDepth);
	void setWireframeMode(Bool isWireframeMode);
	void addRenderObject(RenderObjectNames renderObjectNames);
	
	Area getArea();
	Bool getCulling();
	Float deltaTime();
	Float getAspectRatio();
	Uniforms& uniforms();
};