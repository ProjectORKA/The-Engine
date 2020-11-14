#pragma once

#include "Camera.hpp"
#include "RenderObjectSystem.hpp"
#include "GraphicsAPI.hpp"
#include "Debug.hpp"

struct Framebuffer {
	Int width = 1600;
	Int height = 900;

	Short samples = 0;

	Color backgroundColor = Color(0.05f, 0.05f, 0.05f, 0.75f);

	GLuint framebufferID = 0;

	GPUTexture colorTexture;
	GPUTexture depthTexture;

	void use();
	void render();
	void create();
	void destroy();
	void detachTextures();
	void resize(Vec2 & resolution);
	void changeSamples(UShort samples);
	void attachTexture(GPUTexture& texture);
};

struct FramebufferSystem {
	Vec2 adaptiveResolution = Vec2(1600, 900);
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;
	
	void create();
	void destroy();
	void deselect();
	void updateFramebufferSizes();
	void select(Index framebufferIndex);
	void add(Vec2 resolution, UShort samples);

	Framebuffer& current();
};