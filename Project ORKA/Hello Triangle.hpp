#pragma once

#include "Game.hpp"

struct HelloTriangleRenderer final : GameRenderer
{
	OpenGLVertexArrayObject vao;
	OpenGLIndexBuffer       indVBO;
	OpenGLVertexBuffer      posVBO;
	OpenGLVertexBuffer      colVBO;

	void create(Window& window) override;
	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;

	void renderScene(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void renderInteractive(Window& window) override;
};

struct HelloTriangle
{
	HelloTriangleRenderer renderer;

	void run()
	{
		ui.window("Hello Triangle", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
