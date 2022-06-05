
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
	void render(Renderer& renderer);
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

	DNDWorld() {
		load();
	}
	~DNDWorld() {
		save();
	}
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

	DNDRenderer(DNDWorld& world);
	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

Int diceRoll(Int diceCount);