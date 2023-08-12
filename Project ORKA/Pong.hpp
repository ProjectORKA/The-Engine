#pragma once
#include "Game.hpp"
struct Ball;
struct Renderer;
struct Window;
#define PONG_DIFFICULTY_MODIFIER 1.05

struct PongPlayer
{
	UInt score     = 0;
	Vec2 position  = Vec2(0, 0);
	Vec2 direction = Vec2(1, 0);
	Bool shoot     = false;
	// ai stuff
	Float target     = 0.0f;
	Float velocity   = 0.0f;
	Float difficulty = 7.0f;
	void  update(Window& window);
	void  mouseInput(Vec3 cursorWorldPosition);
	void  ballLocationInput(Vector<Ball>& balls);
	void  aiInput(Vector<Ball>& balls, Float deltaTime);
};

struct Ball
{
	Float desiredSpeed   = 2;
	Bool  stuckToPaddle  = true;
	Bool  stuckToPaddle1 = true;
	Vec2  position       = Vec2(0);
	Vec2  velocity       = Vec2(1, 0);
	void  update(Float deltaTime, PongPlayer players[2]);
	void  render(ResourceManager& resourceManager, Renderer& renderer) const;
};

struct PongRenderer : GameRenderer
{
	Vector<Ball> balls;
	PongPlayer   players[2];
	InputId      shootButton1secondary = InputId(InputType::Mouse, 0);
	InputId      moveUpButton1         = InputId(InputType::KeyBoard, W);
	InputId      moveDownButton1       = InputId(InputType::KeyBoard, S);
	InputId      moveUpButton2         = InputId(InputType::KeyBoard, UP);
	InputId      moveDownButton2       = InputId(InputType::KeyBoard, DOWN);
	InputId      shootButton2          = InputId(InputType::KeyBoard, LEFT);
	InputId      shootButton1          = InputId(InputType::KeyBoard, SPACE);
	void         update(Window& window) override;
	void         destroy(Window& window) override;
	void         inputEvent(Window& window, InputEvent input) override;
	void         create(ResourceManager& resourceManager, Window& window) override;
	void         render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void         renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct Pong
{
	UserInterface   ui;
	PongRenderer    pongRenderer;
	ResourceManager resourceManager;
	void            run();
};

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls);
