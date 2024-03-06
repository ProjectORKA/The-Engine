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
#include "PointRenderer.hpp"
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
	PointRenderer      pointRenderer;
	TextureSystem      textureSystem;
	TextRenderSystem   textRenderSystem;
	RectangleRenderer  rectangleRenderer;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	//general usage
	void create();
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
	void useTexture(const Name& name);
	void useTexture(const Name& name, Index location);

	//meshes
	void rerenderMesh();
	void renderMesh(Index meshId);
	void useMesh(const Name& name);
	void renderMesh(const Name& name);
	void renderMeshInstanced(const Name& name, const Vector<Vec2>& positions);
	void renderMeshInstanced(const Name& name, const Vector<Vec3>& positions);
	void renderMeshInstanced(const Name& name, const Vector<Matrix>& transforms);
	void renderMeshInstanced(const Name& name, const Vector<Vec3>& positions, Float size);

	//shaders
	void  fill(Vec3 color);
	void  fill(Vec4 color);
	void  setColor(Color color);
	void  fill(Float r, Float g, Float b);
	void  clearBackground(Color color) const;
	Index useShader(const Name& name);
	void  fullScreenShader(const Name& name);
	void  postProcess(const Name& name, const Framebuffer& source, const Framebuffer& destination);

	//primitives
	void centeredCube(Float size)
	{
		Bool tmp = wireframeMode;
		setWireframeMode(true);
		useShader("color");
		fill(Vec4(1, 0, 0, 1));
		uniforms().setMMatrix(matrixFromSize(size * 2));
		renderMesh("centeredCube");
		setWireframeMode(false);
		wireframeMode = tmp;
	}
	void line(Vec2 start, Vec2 end);
	void line(Vec3 start, Vec3 end);
	void arrow(Vec2 start, Vec2 end);
	void arrow(Vec3 start, Vec3 end);
	void line(const Vector<Vec2>& line);
	void line(const Vector<Vec3>& line);
	void circle(Vec2 pos, Float radius);
	void renderSky(const Camera& camera);
	void lines(const Vector<Vec2>& lines);
	void lines(const Vector<Vec3>& lines);
	void lines(const Vector<Line3D>& lines);
	void points(const Vector<Vec2>& points);
	void points(const Vector<Vec3>& points);
	void renderAtmosphere(const Player& player, Vec3 sunDirection);
	void rectangle(Vec2 pos, Vec2 size, Bool overrideColor = true, Bool centered = true);

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
