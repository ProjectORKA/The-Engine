#pragma once
#include "Framebuffer.hpp"
#include "MatrixSystem.hpp"
#include "MeshSystem.hpp"
#include "PrimitivesRenderer.hpp"
#include "RenderObjectSystem.hpp"
#include "RenderRegion.hpp"
#include "ShaderSystem.hpp"
#include "TextRenderSystem.hpp"
#include "TextureSystem.hpp"
#include "Time.hpp"

struct Camera;
struct Player;
struct Window;

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
	//RenderObjectSystem renderObjectSystem;

	OpenGlContext openGlContext;

	//general usage
	void sync();
	void create();
	void destroy();
	void begin(IVec2 size);
	//render state
	void drawToWindow();
	void enableScissor();
	void disableScissor();
	void setWireframeMode();
	void blendModeAdditive();
	void end(Window & window);
	void pollGraphicsApiError();
	void setCulling(Bool culling);
	void setWireframeMode(Bool mode);
	void setAlphaBlending(Bool blending);
	void setDepthTest(Bool isUsingDepth);
	void setRenderRegion(TiledRectangle region);
	void setScissorTest(IVec2 position, IVec2 size);
	void setRenderRegion(IVec2 position, IVec2 size);
	void setFramebufferSize(Renderer& renderer, IVec2 area);
	void setAlphaBlending(Bool enable, BlendFunction src, BlendFunction dst, BlendEquation eq);

	//spaces
	void normalizedSpace();									// stretched -1 and 1 to window
	void screenSpaceFromBottomLeft();						// pixel perfect coordinates in positive x and y
	void uiSpace(const Window& window);						// similar to how browsers work
	void aspectCorrectNormalizedSpace();					// fits -1 and 1 into window
	void screenSpaceFromTopLeft(const Window & window);		// pixel perfect coordinates in positive x and negative y
	void normalizedScreenSpace(const Window & window);		//
	void normalizedSpaceWithAspectRatio(Float aspectRatio);	//

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
	void renderMeshInstanced(const Name& name, const Vec3Vector& positions, const Vec2Vector& directions, const Vector<Float>& scales);

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
	void lines(const Vec3Vector& lines);
	void lines(const Vec2Vector& lines);
	void renderSky(const Camera& camera);
	void rectangle(IVec2 pos, IVec2 size);
	void points(const Vec2Vector& points);
	void points(const Vec3Vector& points);
	void wireframeCubeCentered(Float size);
	void circles(const Vector<Matrix>& matrices);
	void wireframeCubes(const Vector<Matrix>& matrices);
	void line(const Vec2Vector& line, const Matrix& matrix);
	void lines(const Vec2Vector& lines, const Matrix& matrix);
	void renderAtmosphere(const Player& player, Vec3 sunDirection);
	void renderUITexture(IVec2 position, IVec2 size);
	void UIRectangle(IVec2 pos, IVec2 size);

	//text
	void text(const String& text);
	void fontSize(const Float size);
	void letterSpacing(const Float spacing);
	void text(const String& text, const Vec2& position);

	[[nodiscard]] Float  deltaTime() const;
	[[nodiscard]] Bool   getCulling() const;
	[[nodiscard]] Float  getAspectRatio() const;
	[[nodiscard]] Matrix getScreenSpaceMatrix() const;
	[[nodiscard]] Matrix getUISpaceMatrix(const Window & window) const;

	Uniforms& uniforms();

private:
	RenderRegion renderRegion;
	//IVec2        windowSize = IVec2(0);
};
