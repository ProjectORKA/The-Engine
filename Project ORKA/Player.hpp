#pragma once

#include "Camera.hpp"
#include "Input.hpp"

struct Window;

//basic implementation of a player that can move in all directions at a defined speed
//inherit from this to make your own player
struct Player {
	Camera camera;
	Double mouseSensitivity = 0.0015;

	//input
	InputID forward = InputID(InputType::KeyBoard, W);
	InputID backward = InputID(InputType::KeyBoard, S);
	InputID right = InputID(InputType::KeyBoard, D);
	InputID left = InputID(InputType::KeyBoard, A);
	InputID up = InputID(InputType::KeyBoard, E);
	InputID down = InputID(InputType::KeyBoard, Q);

	//both functions need to be run in order for the player to work
	//call update once in order to update player logic such as movement
	virtual void update(Window& window);
	//call render if you want to render from the players perspective
	virtual void render(Window& window);
	//this gets called for every input event
	virtual void inputEvent(Window& window, InputEvent input) {};
};

struct DebugPlayer : public Player {
	//speed is calculated exponentially which is perfect for debugging purposes 
	//you can zoom around at lightspeed or be slow as a snake
	Int speedExponent = 0; //how fast are we going
	Float baseNumber = 1.2; //how much does speed change

	//input for speeding up / slowing down
	InputEvent faster = InputEvent(InputType::Scroll, 1, 1);
	InputEvent slower = InputEvent(InputType::Scroll, 1, 0);

	//overrides update and inputEvent to work
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
};