
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
	void apectCorrectNormalizedSpace();
	
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
	Index useShader(Name name) {
		return shaderSystem.use(name);
	}

	//primitives
	void line2D(Vec2 start, Vec2 end, Float width) {
		
		Vec2 dir = normalize(start - end);
		Vec2 extend = cross(Vec3(dir, 0),Vec3(0,0,1));
		
		CPUMesh line;
		line.drawMode = MeshDrawMode::dynamicMode;
		line.indices.push_back(0);
		line.indices.push_back(1);
		line.indices.push_back(2);
		line.indices.push_back(3);
		line.name = "line";
		line.normals.push_back(Vec3(0, 0, 1));
		line.normals.push_back(Vec3(0, 0, 1));
		line.normals.push_back(Vec3(0, 0, 1));
		line.normals.push_back(Vec3(0, 0, 1));
		line.primitiveMode = PrimitiveMode::TriangleStrip;
		line.uvs.push_back(Vec2(0, 1));
		line.uvs.push_back(Vec2(0, 0));
		line.uvs.push_back(Vec2(1, 0));
		line.uvs.push_back(Vec2(1, 1));
		line.vertices.push_back(Vec3(start + extend * width, 0));
		line.vertices.push_back(Vec3(start - extend * width, 0));
		line.vertices.push_back(Vec3(end + extend * width, 0));
		line.vertices.push_back(Vec3(end - extend * width, 0));
		line.checkIntegrity();

		GPUMesh gpuMesh;
		gpuMesh.upload(line);
		gpuMesh.render();
		gpuMesh.unload();
	};
	void arrow2D(Vec2 start, Vec2 end, Float width) {

		Vec2 dir = normalize(start - end);
		Vec2 extend = cross(Vec3(dir, 0), Vec3(0, 0, 1));

		CPUMesh arrow;
		arrow.drawMode = MeshDrawMode::dynamicMode;
		arrow.indices.push_back(0);
		arrow.indices.push_back(1);
		arrow.indices.push_back(2);
		arrow.indices.push_back(3);
		arrow.name = "arrow";
		arrow.normals.push_back(Vec3(0, 0, 1));
		arrow.normals.push_back(Vec3(0, 0, 1));
		arrow.normals.push_back(Vec3(0, 0, 1));
		arrow.normals.push_back(Vec3(0, 0, 1));
		arrow.primitiveMode = PrimitiveMode::TriangleStrip;
		arrow.uvs.push_back(Vec2(0, 1));
		arrow.uvs.push_back(Vec2(0, 0));
		arrow.uvs.push_back(Vec2(1, 0));
		arrow.uvs.push_back(Vec2(1, 1));
		arrow.vertices.push_back(Vec3(start + extend * width, 0));
		arrow.vertices.push_back(Vec3(start - extend * width, 0));
		arrow.vertices.push_back(Vec3(end, 0));
		arrow.vertices.push_back(Vec3(end, 0));
		arrow.checkIntegrity();

		GPUMesh gpuMesh;
		gpuMesh.upload(arrow);
		gpuMesh.render();
		gpuMesh.unload();
	};

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

void loadPrimitives(Renderer& renderer);