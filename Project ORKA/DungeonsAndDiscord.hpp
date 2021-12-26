
#pragma once

#include "Transform.hpp"
#include "Game.hpp"
#include "KeyMap.hpp"
#include "Player.hpp"
#include "FileSystem.hpp"

struct Renderer;
struct Window;

struct Object {
	Transform transform;
	Name meshName = "";
	void render(Renderer& renderer);;
};

struct DungeonsAndDiscord : public Game {
	
	//Input
	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;
	
	Float mouseSensitivity = 0.0015f;

	//player
	Player player;

	//world
	Vector<Object> objects;
	List<Path> importObjects;

	void update() override;
	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void filesDropped(Window& window, Vector<Path> paths) override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
};

Int diceRoll(Int diceCount);