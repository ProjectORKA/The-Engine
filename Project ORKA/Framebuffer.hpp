#pragma once

#include "Camera.hpp"
#include "RenderObjectSystem.hpp"
#include "GraphicsAPI.hpp"
#include "Debug.hpp"

struct Framebuffer {
	Int width = 1600;
	Int height = 900;

	Color backgroundColor = Color(0.05f, 0.05f, 0.05f, 0.75f);

	GLuint framebufferID = 0;

	Index colorTextureIndex = 0;
	GLuint depthTextureIndex = 0;

	void create(TextureSystem & textureSystem);
	void update(TextureSystem & textureSystem, Vec2 & resolution);
	void use();
	void render();
	void renderAdvanced();
	void clear();
	void destroy();
};

struct FramebufferSystem {
	Vec2 adaptiveResolution = Vec2(1600, 900);
	Vector<Framebuffer> framebuffers;
	Index currentFramebufferIndex = 0;
	void create(RenderObjectSystem & renderObjectSystem);
	void add(Vec2 resolution, TextureSystem & textureSystem);
	void updateFramebuffers(TextureSystem& textureSystem);
	void select(Index framebufferIndex);
	void selectFinal();
	Framebuffer& current();
};