
#pragma once

#include "Transform.hpp"
#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "ECS.hpp"

// todo list
// import objects
// move objects around
// also scale and rotate them in real time
// be able to change the sky

struct Renderer;
struct Window;

struct DNDEntity {
	Name meshName = "empty";
	Transform transform;
	void render(Engine& engine, Renderer& renderer);
};

struct DNDWorld : public GameSimulation {
	//savedata for camera
	Float fieldOfView = 80.0;
	Float nearClipValue = 0.1;
	Float farClipValue = 100000;
	Vec3 location = Vec3(0);
	DVec3 rotation = Vec3(0);
	//player data
	Int speedExponent = 1;
	
	Vector<DNDEntity> entities;

	void load();
	void save();
};

struct DNDRenderer : public GameRenderer {
	DebugPlayer player;
	DNDWorld* world = nullptr;
	Index lastSelectedObject = -1;
	Vector<Index> selectedObjects;
	Float mouseSensitivity = 0.0015f;
	InputEvent select = InputEvent(InputType::Mouse, LMB, 1);
	InputID selectMultiple = InputID(InputType::KeyBoard, SHIFT);

	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void create(Engine& engine, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Engine& engine, Window& window, TiledRectangle area) override;
	void renderInteractive(Engine& engine, Window& window, TiledRectangle area) override;
};

struct DungeonsAndDiscord {
	DNDWorld sim;
	Engine engine;
	UserInterface ui;
	DNDRenderer renderer;

	void run() {
		renderer.world = &sim;
		ui.window("Simple RTS", Area(1920, 1080), true, WindowState::windowed, renderer, engine);
		ui.run();
	}
};

Int diceRoll(Int diceCount);