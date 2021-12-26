
#pragma once

#include "Game.hpp"

struct Ball;
struct Renderer;
struct Window;

struct PongPlayer {
	ULL score = 0;
	Vec2 position = Vec2(0,0);

	//actions
	Bool shoot = false;
	Bool moveUp = false;
	Bool moveDown = false;

	//ai stuff
	Float target = 0.0f;
	Float velocity = 0.0f;
	Float difficulty = 7.0f;

	void keyboardInput(Float deltaTime);
	void mouseInput(Vec3 cursorWorldPosition);
	void ballLocationInput(Vector<Ball>& balls);
	void aiInput(Vector<Ball>& balls, Float deltaTime);
};

struct Ball {
	//ball
	Vec2 position = Vec2(0);
	Vec2 velocity = Vec2(1, 0);
	Float speed = 2;
	Bool stuckToPaddle = true;
	Bool stuckToPaddle1 = true;

	void render(Renderer& renderer);
	void update(Float deltaTime, PongPlayer players[2]);

};

struct Pong : public Game {
	using Game::Game;

	//players
	PongPlayer players[2];

	Vector<Ball> balls;

	void render(Renderer& renderer) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;;
};

Ball* getClosestBall(PongPlayer& player, Vector<Ball>& balls);