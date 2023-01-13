
#pragma once

#include "Game.hpp"

struct Ball;
struct Renderer;
struct Window;

#define PONG_DIFFICULTY_MODIFIER 1.05

struct PongPlayer {
	ULL score = 0;
	Vec2 position = Vec2(0,0);

	Bool shoot = false;

	//ai stuff
	Float target = 0.0f;
	Float velocity = 0.0f;
	Float difficulty = 7.0f;

	void update(Window& window);
	void mouseInput(Vec3 cursorWorldPosition);
	void ballLocationInput(Vector<Ball>& balls);
	void aiInput(Vector<Ball>& balls, Float deltaTime);
};

struct Ball {
	//ball
	Vec2 position = Vec2(0);
	Vec2 velocity = Vec2(1, 0);
	Float desiredSpeed = 2;
	Bool stuckToPaddle = true;
	Bool stuckToPaddle1 = true;

	void render(Engine& engine, Renderer& renderer);
	void update(Float deltaTime, PongPlayer players[2]);
};

struct Pong : public GameRenderer {
	//players
	PongPlayer players[2];

	InputID moveUpButton1 = InputID(InputType::KeyBoard, W);
	InputID moveDownButton1 = InputID(InputType::KeyBoard, S);
	InputID shootButton1 = InputID(InputType::KeyBoard, SPACE);
	InputID shootButton1secondary = InputID(InputType::Mouse, 0);

	InputID moveUpButton2 = InputID(InputType::KeyBoard, UP);
	InputID moveDownButton2 = InputID(InputType::KeyBoard, DOWN);
	InputID shootButton2 = InputID(InputType::KeyBoard, LEFT);

	Vector<Ball> balls;
	void update(Window& window) override;
	void render(Engine& engine, Window& window, TiledRectangle area) override;
};

Ball* getClosestBall(PongPlayer& player, Vector<Ball>& balls);