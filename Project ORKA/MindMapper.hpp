
#pragma once

#include "Game.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

struct MindMapper : public GameRenderer {
	Mutex mutex;
	DebugPlayer player;

	MindMap system;

	Bool grow = false;

	InputEvent exit = InputEvent(InputType::Mouse, 1, 0);
	InputEvent enter = InputEvent(InputType::Mouse, 0, 1);
	InputEvent growToggle = InputEvent(InputType::KeyBoard, SPACE, 0);

	MindMapper() {};
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine& engine, Window& window, TiledRectangle area) override;
};