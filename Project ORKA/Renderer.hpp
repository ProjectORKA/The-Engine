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
//#include "PointRenderer.hpp"
#include "TextRenderSystem.hpp"
//#include "RectangleRenderer.hpp"
#include "RenderObjectSystem.hpp"
#include "PlanetRenderSystem.hpp"

struct Camera;
struct Player;

struct PrimitivesRenderer
{
	// [TODO]: check if pointsMesh is rendered using indices or if they can be removed
private:
	GPUMesh lineMesh;
	GPUMesh linesMesh;
	GPUMesh cubeMesh;
	GPUMesh planeMesh;
	GPUMesh pointsMesh;

public:

	void create()
	{
		CPUMesh genMesh;

		// points

		genMesh.drawMode      = BufferUsage::DynamicDraw;
		genMesh.name          = "PointCloud";
		genMesh.primitiveMode = PrimitiveMode::Points;
		genMesh.positions.emplace_back(0.0f,0.0f,0.0f);
		genMesh.checkIntegrity();
		pointsMesh.upload(genMesh);
		genMesh.clearGeometry();

		// line

		genMesh.drawMode      = BufferUsage::DynamicDraw;
		genMesh.name          = "Line";
		genMesh.primitiveMode = PrimitiveMode::LineStrip;
		genMesh.positions.emplace_back(-0.5f,0.0f,0.0f);
		genMesh.positions.emplace_back(0.5f,0.0f,0.0f);
		genMesh.indices.emplace_back(0);
		genMesh.indices.emplace_back(1);
		genMesh.checkIntegrity();
		lineMesh.upload(genMesh);
		genMesh.clearGeometry();

		// lines

		genMesh.drawMode      = BufferUsage::DynamicDraw;
		genMesh.name          = "Lines";
		genMesh.primitiveMode = PrimitiveMode::Lines;
		genMesh.positions.emplace_back(-0.5f,0.0f,0.0f);
		genMesh.positions.emplace_back(0.5f,0.0f,0.0f);
		genMesh.indices.emplace_back(0);
		genMesh.indices.emplace_back(1);
		genMesh.checkIntegrity();
		linesMesh.upload(genMesh);
		genMesh.clearGeometry();

		// plane / rectangle

		genMesh.drawMode      = BufferUsage::StaticDraw;
		genMesh.name          = "Plane";
		genMesh.primitiveMode = PrimitiveMode::Triangles;
		genMesh.positions.emplace_back(-0.5f,-0.5f,0);
		genMesh.positions.emplace_back(0.5f,-0.5f,0);
		genMesh.positions.emplace_back(-0.5f,0.5f,0);
		genMesh.positions.emplace_back(0.5f,0.5f,0);

		genMesh.textureCoordinates.emplace_back(0,0);
		genMesh.textureCoordinates.emplace_back(1,0);
		genMesh.textureCoordinates.emplace_back(0,1);
		genMesh.textureCoordinates.emplace_back(1,1);

		genMesh.indices.emplace_back(0);
		genMesh.indices.emplace_back(1);
		genMesh.indices.emplace_back(2);
		genMesh.indices.emplace_back(2);
		genMesh.indices.emplace_back(1);
		genMesh.indices.emplace_back(3);

		genMesh.checkIntegrity();
		planeMesh.upload(genMesh);
		genMesh.clearGeometry();
    }

	void wireframeCube(Uniforms& uniforms)
	{
		const Vector<Vec3>  positions = {Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f), Vec3(-0.5f, -0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f),};
		const Vector<Index> indices   = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
		lines(positions, indices, uniforms);
	}

	void points(const Vector<Vec3> & pos, Uniforms& uniforms)
	{
		Vector<Index> indices;
		for(int i = 0; i < pos.size(); i++) indices.emplace_back(i);
		pointsMesh.updatePositionBuffer(pos);
		pointsMesh.updateIndexBuffer(indices);
		pointsMesh.render(uniforms);
	}

	void points(const Vector<Vec2> & pos, Uniforms& uniforms)
	{
		points(vec2VectorToVec3Vector(pos), uniforms);
	}

	void rectangle(Uniforms& uniforms) const
	{
		planeMesh.render(uniforms);
	}

	void line(const Vector<Vec2>& positions, Uniforms& uniforms)
	{
		line(vec2VectorToVec3Vector(positions),uniforms);
	}

	void line(const Vector<Vec3>& positions, Uniforms& uniforms)
	{
		Vector<Index> indices;
		for(int i = 0; i < positions.size(); i++) indices.emplace_back(i);
		lineMesh.updatePositionBuffer(positions);
		lineMesh.updateIndexBuffer(indices);
		lineMesh.render(uniforms);
	}

	void lines(const Vector<Vec2>& positions, Uniforms& uniforms)
	{
		lines(vec2VectorToVec3Vector(positions),uniforms);
	}

	void lines(const Vector<Vec3>& positions, Uniforms& uniforms)
	{
		Vector<Index> indices;
		for(int i = 0; i < positions.size(); i++) indices.emplace_back(i);
		lines(positions, indices, uniforms);
	}

	void lines(const Vector<Vec3>& positions, const Vector<Index>& indices, Uniforms& uniforms)
	{
		linesMesh.updatePositionBuffer(positions);
		linesMesh.updateIndexBuffer(indices);
		linesMesh.render(uniforms);
	}

	void destroy()
	{
		lineMesh.unload();
		linesMesh.unload();
		planeMesh.unload();
		pointsMesh.unload();
	}
};

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

	//general usage
	void end();
	void sync();
	void create();
	void destroy();
	void begin(Area size);

	//render state
	void drawToWindow() const;
	void blendModeAdditive() const;
	void pollGraphicsApiError() const;
	void setWireframeMode() const;
	void setCulling(Bool culling) const;
	void setFramebufferSize(Area area);
	void setAlphaBlending(Bool blending) const;
	void setDepthTest(Bool isUsingDepth) const;
	void setWireframeMode(Bool mode) const;
	void setRenderRegion(TiledRectangle region);
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
	void useTexture(const Name& name, Index position);

	//meshes
	void rerenderMesh();
	void renderMesh(Index meshId);
	void useMesh(const Name& name);
	void renderMesh(const Name& name);
	void renderMeshInstanced(const Name& name, const Vector<Vec2>& positions);
	void renderMeshInstanced(const Name& name, const Vector<Vec3>& positions);
	void renderMeshInstanced(const Name& name, const Vector<Matrix>& transforms);
	void renderMeshInstanced(const Name& name, const Vector<Vec3>& positions, Float size);
	void renderMeshInstanced(const Name& name, const Vector<Vec2>& positions, const Vector<Vec2>& directions, const Vector<Float>& scales);

	//shaders
	void  fill(Int color);
	void  fill(Vec3 color);
	void  fill(Vec4 color);
	void  setColor(Color color);
	Index useShader(const Name& name);
	void  clearBackground(Color color) const;
	void  fill(Float r, Float g, Float b);
	void  fullScreenShader(const Name& name);
	void  postProcess(const Name& name, const Framebuffer& source, const Framebuffer& destination);

	//primitives
	void centeredCube(Float size);
	void line(Vec2 start, Vec2 end);
	void line(Vec3 start, Vec3 end);
	void arrow(Vec2 start, Vec2 end);
	void arrow(Vec3 start, Vec3 end);
	void line(const Vector<Vec2>& line);
	void line(const Vector<Vec3>& line);
	void circle(Vec2 pos, Float radius);
	void renderSky(const Camera& camera);
	void lines(const Vector<Vec3>& lines);
	void lines(const Vector<Vec2>& lines);
	//void lines(const Vector<Line3D>& lines);
	void points(const Vector<Vec2>& points);
	void points(const Vector<Vec3>& points);
	void line(const Vector<Vec2>& line, const Matrix& matrix);
	void lines(const Vector<Vec2>& lines, const Matrix& matrix);
	void renderAtmosphere(const Player& player, Vec3 sunDirection);
	//void rectangle(Vec2 pos, Vec2 size, Bool overrideColor = true, Bool centered = true);

	[[nodiscard]] Bool   getCulling() const;
	[[nodiscard]] Area   getArea() const;
	[[nodiscard]] Float  deltaTime() const;
	[[nodiscard]] Area   getWindowSize() const;
	[[nodiscard]] Float  getAspectRatio() const;
	[[nodiscard]] Int    getWindowWidth() const;
	[[nodiscard]] Int    getWindowHeight() const;
	[[nodiscard]] Matrix getScreenSpaceMatrix() const;

	Uniforms& uniforms();
	void plane()
	{
		primitivesRenderer.rectangle(uniforms());
	}

	void wireframeCube()
	{
		primitivesRenderer.wireframeCube(uniforms());
	}

private:
	RenderRegion renderRegion;
	Area         windowSize = Area(0);
};

class DepthTestOverride
{
public:
	~DepthTestOverride();
	DepthTestOverride() = delete;
	DepthTestOverride(Bool value, Renderer& renderer);

private:
	Bool      stored;
	Renderer* renderer;
};
