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
	void connect(GameSimulation& simulation) override;
	void create(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window, TiledRectangle area) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct HelloTriangle
{
	UserInterface         ui;
	Window                window;
	HelloTriangleRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("Hello Triangle", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
