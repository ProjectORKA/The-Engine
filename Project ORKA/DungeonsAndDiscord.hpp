#pragma once

#include "Transform.hpp"
#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "ECS.hpp"
#include "SceneSystem.hpp"

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
	void render(ResourceManager& resourceManager, Renderer& renderer);
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

	Scene2 scene;

	Vector<DNDEntity> entities;

	void load(ResourceManager& resourceManager);
	void save();
};

struct DNDRenderer : public GameRenderer {
	DebugPlayer player;
	DNDWorld* world = nullptr;
	Index lastSelectedObject = -1;
	Vector<Index> selectedObjects;
	Float mouseSensitivity = 0.0015f;
	InputEvent select = InputEvent(InputType::Mouse, LMB, true);
	InputId selectMultiple = InputId(InputType::KeyBoard, SHIFT);

	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);

	void update(Window& window) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct DungeonsAndDiscord {
	DNDWorld sim;
	ResourceManager resourceManager;
	UserInterface ui;
	DNDRenderer renderer;

	void run();
};

Int diceRoll(Int diceCount);
