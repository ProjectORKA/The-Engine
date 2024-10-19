#pragma once

#include "Time.hpp"
#include "Basics.hpp"
#include "Settings.hpp"
#include "MeshSystem.hpp"
#include "RenderRegion.hpp"
#include "ShaderSystem.hpp"
#include "MatrixSystem.hpp"
#include "TextureSystem.hpp"
#include "TextRenderSystem.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"
#include "PrimitivesRenderer.hpp"
#include "RenderingOverrides.hpp"

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
	TextureSystem      textureSystem;
	TextRenderSystem   textRenderSystem;
	PrimitivesRenderer primitivesRenderer;
	RenderObjectSystem renderObjectSystem;
	PlanetRenderSystem planetRenderSystem;

	OpenGlContext openGlContext;

	//general usage
	void end();
	void sync();
	void create();
	void destroy();
	void begin(Area size);

	//render state
	void blendModeAdditive();
	void drawToWindow() const;
	void setCulling(Bool culling);
	void setWireframeMode() const;
	void pollGraphicsApiError() const;
	void setAlphaBlending(Bool blending);
	void setDepthTest(Bool isUsingDepth);
	void setWireframeMode(Bool mode) const;
	void setRenderRegion(TiledRectangle region);
	void setFramebufferSize(Renderer& renderer, Area area);
	void addRenderObject(const RenderObjectNames& renderObjectNames); // [TODO] remove?
	void setAlphaBlending(Bool enable, BlendFunction src, BlendFunction dst, BlendEquation eq);

	//spaces
	void screenSpace();
	void normalizedSpace();
	void normalizedScreenSpace();
	void screenSpaceFromTopLeft();
	void aspectCorrectNormalizedSpace();
	void normalizedSpaceWithAspectRatio(Float aspectRatio);

	//textures
	void useTexture(const Name& name);
	void useTexture(const Name& name, Index position);

	//meshes
	//void rerenderMesh();
	void renderMesh(Index meshId);
	void useMesh(const Name& name);
	void renderMesh(const Name& name);
	void renderMeshInstanced(const Name& name, const Vec2Vector& positions);
	void renderMeshInstanced(const Name& name, const Vec3Vector& positions);
	void renderMeshInstanced(const Name& name, const Vector<Matrix>& transforms);
	void renderMeshInstanced(const Name& name, const Vec3Vector& positions, Float size);
	void renderMeshInstanced(const Name& name, const Vec2Vector& positions, const Vec2Vector& directions, const Vector<Float>& scales);

	//shaders
	void  fill(Int color);
	void  fill(Vec3 color);
	void  fill(Vec4 color);
	void  fill(Float color);
	void  fill(Double color);
	void  setColor(Color color);
	Index useShader(const Name& name);
	void  clearBackground(Color color);
	void  fill(Float r, Float g, Float b);
	void  fullScreenShader(const Name& name);
	void  fill(Float r, Float g, Float b, Float a);
	void  postProcess(const Name& name, const Framebuffer& source, const Framebuffer& destination);

	//primitives
	void plane();
	void rectangle();
	void wireframeCube();
	void wireframeCubeCentered();
	void line(Vec2 start, Vec2 end);
	void line(Vec3 start, Vec3 end);
	void arrow(Vec2 start, Vec2 end);
	void arrow(Vec3 start, Vec3 end);
	void line(const Vec2Vector& line);
	void line(const Vec3Vector& line);
	void circle(Vec2 pos, Float radius);
	void renderSky(const Camera& camera);
	void lines(const Vec3Vector& lines);
	void lines(const Vec2Vector& lines);
	void wireframeCubeCentered(Float size);
	void points(const Vec2Vector& points);
	void points(const Vec3Vector& points);
	void circles(const Vector<Matrix>& matrices);
	void wireframeCubes(const Vector<Matrix>& matrices);
	void line(const Vec2Vector& line, const Matrix& matrix);
	void lines(const Vec2Vector& lines, const Matrix& matrix);
	void renderAtmosphere(const Player& player, Vec3 sunDirection);

	[[nodiscard]] Bool   getCulling() const;
	[[nodiscard]] Area   getArea() const;
	[[nodiscard]] Float  deltaTime() const;
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
