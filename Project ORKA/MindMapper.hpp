#pragma once

#include "Game.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

struct MindMapperRenderer final : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;

	MindMap system;

	Bool grow = false;

	InputEvent exit = InputEvent(InputType::Mouse, 1, false);
	InputEvent enter = InputEvent(InputType::Mouse, 0, true);
	InputEvent growToggle = InputEvent(InputType::KeyBoard, SPACE, false);

	MindMapperRenderer() = default;
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct MindMapper {
	UserInterface ui;
	ResourceManager resourceManager;
	MindMapperRenderer mindMapperRenderer;
	void run();
};
