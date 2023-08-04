#pragma once

#include "Game.hpp"

struct HelloTriangleRenderer final : GameRenderer
{
	OpenGLVertexArrayObject vao;
	OpenGLIndexBuffer       indVBO;
	OpenGLVertexBuffer      posVBO;
	OpenGLVertexBuffer      colVBO;
	Framebuffer             mainFramebuffer;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void create(ResourceManager& rm, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& rm, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct HelloTriangle
{
	UserInterface ui;
	ResourceManager resourceManager;
	HelloTriangleRenderer renderer;

	void run();
};
