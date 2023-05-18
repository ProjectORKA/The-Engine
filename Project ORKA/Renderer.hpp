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


struct Renderer {
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

	void create(ResourceManager& resourceManager, Area size);
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
	void screenSpaceFromTopLeft();
	void aspectCorrectNormalizedSpace();
	void normalizedSpaceWithAspectRatio(Float aspectRatio);

	//meshes
	void rerenderMesh();
	void renderMesh(Index meshID);
	void useMesh(ResourceManager& resourceManager, const Name& name);
	void renderMesh(ResourceManager& resourceManager, const Name& name);
	void renderMeshInstanced(ResourceManager& resourceManager, const Name& name);

	//textures
	void useTexture(ResourceManager& resourceManager, const Name& name);
	void useTexture(ResourceManager& resourceManager, const Name& name, Index location);

	//shaders
	Index useShader(ResourceManager& resourceManager, const Name& name);
	void postProcess(ResourceManager& resourceManager, const Name& name);
	void fullScreenShader(ResourceManager& resourceManager, const Name& name);

	//framebuffers
	void read(const Name& name);
	void draw(const Name& name);

	//colors
	void fill(Vec3 color);
	void fill(Vec4 color);
	void clearColor(Color color);
	void fill(Float r, Float g, Float b);

	//primitives
	void line(Vec3 start, Vec3 end, Float width);
	void line(Vec2 start, Vec2 end, Float width);
	void arrow(ResourceManager& resourceManager, Vec2 start, Vec2 end);
	void arrow(ResourceManager& resourceManager, Vec3 start, Vec3 end);
	void rectangle(ResourceManager& resourceManager, Vec2 pos, Vec2 size);
	void circle(ResourceManager& resourceManager, Vec2 pos, Float radius);

	//sky
	void renderSky(ResourceManager& resourceManager, Camera& camera);
	void renderAtmosphere(ResourceManager& resourceManager, Player& player, Vec3 sunDirection);

	//blending
	void blendModeAdditive();
	void setAlphaBlending(Bool blending);
	void setAlphaBlending(Bool enable, BlendFunction src, BlendFunction dst, BlendEquation eq);

	void setWireframeMode();
	void setColor(Color color);
	void pollGraphicsAPIError() const;
	void setCulling(Bool isCulling);
	//void setDepthClamp(Bool depthClamp); //[TODO] maybe use depth clamp by default, it should allow for infinite range with proper depth testing in the desired range
	void setDepthTest(Bool isUsingDepth);
	void setWireframeMode(Bool isWireframeMode);
	void addRenderObject(const RenderObjectNames& renderObjectNames);

	Area getArea() const;
	Bool getCulling();
	Float deltaTime() const;
	Float getAspectRatio();
	Uniforms& uniforms();
	Matrix getScreenSpaceMatrix() const;
	void lines(const Vector<Line3D>& lines) { lineRenderer.renderLines(*this, lines); };
};

struct DepthTestOverride {
	Bool stored = false;
	Renderer* renderer = nullptr;

	DepthTestOverride(const Bool value, Renderer& renderer) {
		this->renderer = &renderer;
		stored = openglState.depthTest;
		this->renderer->setDepthTest(value);
	};

	~DepthTestOverride() { this->renderer->setDepthTest(stored); };
};
