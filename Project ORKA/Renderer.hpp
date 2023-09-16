#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Settings.hpp"
#include "MeshSystem.hpp"
#include "RenderRegion.hpp"
#include "ShaderSystem.hpp"
#include "MatrixSystem.hpp"
#include "LineRenderer.hpp"
#include "TextureSystem.hpp"
#include "TextRenderSystem.hpp"
#include "RectangleRenderer.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"

struct Camera;
struct Player;

struct Renderer
{
	Time        time;
	Mutex       mutex;
	Framebuffer idFramebuffer;
	UInt        objectId    = -1;
	UInt        instanceId  = -1;
	UInt        primitiveId = -1;
	Framebuffer postProcessFramebuffer;
	ULL         frameCount            = 0;
	Bool        adjustRenderVariables = true;
	Bool        wireframeMode         = false;
	Float       targetFrameRate       = settings.targetFrameRate;

	MeshSystem         meshSystem;
	ShaderSystem       shaderSystem;
	MatrixSystem       matrixSystem;
	LineRenderer       lineRenderer;
	TextureSystem      textureSystem;
	TextRenderSystem   textRenderSystem;
	RectangleRenderer  rectangleRenderer;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	//general usage
	void create(ResourceManager& resourceManager);
	void begin(Area windowSize);
	void sync();
	void end();
	void destroy();

	//render state
	void drawToWindow() const;
	void setWireframeMode() const;
	void blendModeAdditive() const;
	void pollGraphicsAPIError() const;
	void setFramebufferSize(Area area);
	void setCulling(Bool culling) const;
	void setAlphaBlending(Bool blending) const;
	void setDepthTest(Bool isUsingDepth) const;
	void setRenderRegion(TiledRectangle region);
	void setWireframeMode(Bool wireframeMode) const;
	void addRenderObject(const RenderObjectNames& renderObjectNames); // [TODO] remove?
	void setAlphaBlending(Bool enable, BlendFunction src, BlendFunction dst, BlendEquation eq) const;

	//spaces
	void screenSpace();
	void normalizedSpace();
	void normalizedScreenSpace();
	void screenSpaceFromTopLeft();
	void aspectCorrectNormalizedSpace();
	void normalizedSpaceWithAspectRatio(Float aspectRatio);

	//textures
	void useTexture(ResourceManager& resourceManager, const Name& name);
	void useTexture(ResourceManager& resourceManager, const Name& name, Index location);

	//meshes
	void rerenderMesh();
	void renderMesh(Index meshId);
	void useMesh(ResourceManager& resourceManager, const Name& name);
	void renderMesh(ResourceManager& resourceManager, const Name& name);
	void renderMeshInstanced(ResourceManager& resourceManager, const Name& name, const Vector<Vec3>& positions);
	void renderMeshInstanced(ResourceManager& resourceManager, const Name& name, const Vector<Matrix>& transforms);
	void renderMeshInstanced(ResourceManager& resourceManager, const Name& name, const Vector<Vec3>& positions, Float size);

	//shaders
	void  fill(Vec3 color);
	void  fill(Vec4 color);
	void  setColor(Color color);
	void  fill(Float r, Float g, Float b);
	void  clearBackground(Color color) const;
	Index useShader(ResourceManager& resourceManager, const Name& name);
	void  fullScreenShader(ResourceManager& resourceManager, const Name& name);
	void  postProcess(ResourceManager& resourceManager, const Name& name, const Framebuffer& source, const Framebuffer& destination);

	//primitives
	void line(Vec2 start, Vec2 end);
	void line(Vec3 start, Vec3 end);
	void line(const List<Vec2>& line);
	void line(const List<Vec3>& line);
	void lines(const Vector<Vec2>& lines);
	void lines(const Vector<Vec3>& lines);
	void lines(const Vector<Line3D>& lines);
	void arrow(ResourceManager& resourceManager, Vec2 start, Vec2 end);
	void arrow(ResourceManager& resourceManager, Vec3 start, Vec3 end);
	void rectangle(ResourceManager& resourceManager, Vec2 pos, Vec2 size);
	void circle(ResourceManager& resourceManager, Vec2 pos, Float radius);
	void renderSky(ResourceManager& resourceManager, const Camera& camera);
	void renderAtmosphere(ResourceManager& resourceManager, const Player& player, Vec3 sunDirection);

	[[nodiscard]] Area   getArea() const;
	[[nodiscard]] Float  deltaTime() const;
	[[nodiscard]] Bool   getCulling() const;
	[[nodiscard]] Area   getWindowSize() const;
	[[nodiscard]] Float  getAspectRatio() const;
	[[nodiscard]] Int    getWindowWidth() const;
	[[nodiscard]] Int    getWindowHeight() const;
	[[nodiscard]] Matrix getScreenSpaceMatrix() const;

	Uniforms& uniforms();

private:
	RenderRegion renderRegion;
	Area         windowSize = Area(0);
};

struct DepthTestOverride
{
	Bool      stored;
	Renderer* renderer;

	~DepthTestOverride();
	DepthTestOverride() = delete;
	DepthTestOverride(Bool value, Renderer& renderer);
};
