#pragma once

#include "Game.hpp"

struct Ball;
struct Renderer;
struct Window;

#define PONG_DIFFICULTY_MODIFIER 1.05

struct PongPlayer
{
	ULL score = 0;
	Vec2 position = Vec2(0, 0);
	Vec2 direction = Vec2(1, 0);

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

struct Ball
{
	//ball
	Vec2 position = Vec2(0);
	Vec2 velocity = Vec2(1, 0);
	Float desiredSpeed = 2;
	Bool stuckToPaddle = true;
	Bool stuckToPaddle1 = true;

	void render(ResourceManager& resourceManager, Renderer& renderer) const;
	void update(Float deltaTime, PongPlayer players[2]);
};

struct PongRenderer : public GameRenderer
{
	//players
	PongPlayer players[2];

	InputId moveUpButton1 = InputId(InputType::KeyBoard, W);
	InputId moveDownButton1 = InputId(InputType::KeyBoard, S);
	InputId shootButton1 = InputId(InputType::KeyBoard, SPACE);
	InputId shootButton1secondary = InputId(InputType::Mouse, 0);

	InputId moveUpButton2 = InputId(InputType::KeyBoard, UP);
	InputId moveDownButton2 = InputId(InputType::KeyBoard, DOWN);
	InputId shootButton2 = InputId(InputType::KeyBoard, LEFT);

	Vector<Ball> balls;
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Pong
{
	ResourceManager resourceManager;
	UserInterface ui;
	PongRenderer pongRenderer;

	void run()
	{
		ui.window("Pong", Area(1920, 1080), true, WindowState::Fullscreen, pongRenderer, resourceManager);
		ui.run();
	};
};

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls);