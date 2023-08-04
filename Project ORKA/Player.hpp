#pragma once

#include "Camera.hpp"
#include "Input.hpp"

struct Window;

// basic implementation of a player that can move in all directions at a defined speed
// inherit from this to make your own player
struct Player
{
	Camera camera;
	Double mouseSensitivity = 0.0015;

	// input
	InputId forward  = InputId(InputType::KeyBoard, W);
	InputId backward = InputId(InputType::KeyBoard, S);
	InputId right    = InputId(InputType::KeyBoard, D);
	InputId left     = InputId(InputType::KeyBoard, A);
	InputId up       = InputId(InputType::KeyBoard, E);
	InputId down     = InputId(InputType::KeyBoard, Q);

	Player()          = default;
	virtual ~Player() = default;
	// both functions need to be run in order for the player to work
	// call update once in order to update player logic such as movement
	virtual void update(Window& window);
	// call render if you want to render from the players perspective
	virtual void render(ResourceManager& resourceManager, Window& window);
	// this gets called for every input event
	virtual void inputEvent(Window& window, InputEvent input);
};

struct DebugPlayer : Player
{
	// speed is calculated exponentially which is perfect for debugging purposes 
	// you can zoom around at light speed or be slow as a snail
	Int   speedExponent = 0; // how fast are we going
	Float baseNumber    = 1.2f; // how much does speed change

	// input for speeding up / slowing down
	InputEvent faster = InputEvent(InputType::Scroll, 1, true);
	InputEvent slower = InputEvent(InputType::Scroll, 1, false);

	// overrides update and inputEvent to work
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;

	DebugPlayer() = default;
};
