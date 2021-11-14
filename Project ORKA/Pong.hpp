
#pragma once

#include "Game.hpp"
#include "Window.hpp"

struct Pong : public Game {
	using Game::Game;

	UInt player1Score = 0;
	UInt player2Score = 0;

	Float paddle1Location = 0;
	Float paddle2Location = 0;

	Vec2 ballPosition = Vec2(0);
	Vec2 ballVelocity = Vec2(1, 0);

	Bool player1MoveUp;
	Bool player1MoveDown;

	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override {
		
		if (keyID == GLFW_KEY_W) {
			player1MoveUp = (action == GLFW_PRESS);
		}

		if (keyID == GLFW_KEY_S) {
			player1MoveDown = (action == GLFW_PRESS);
		}
		
	};

	void update() override {

		//update ball
		ballPosition += ballVelocity;
		ballVelocity = normalize(ballVelocity) * Vec2(0.01);

		//update players
		if (player1MoveUp) paddle1Location += 0.01;
		if (player1MoveDown) paddle1Location -= 0.01;

		//paddles
		if (ballPosition.x > +1 & ballVelocity.x > 0) {
			ballVelocity.x = -ballVelocity.x;
		}
		if (ballPosition.x < -1 & ballVelocity.x < 0) {
			ballVelocity.x = -ballVelocity.x;
		}
		
		//walls
		if (ballPosition.y > +1 & ballVelocity.y > 0) {
			ballVelocity.y = -ballVelocity.y;
		}
		if (ballPosition.y < -1 & ballVelocity.y < 0) {
			ballVelocity.y = -ballVelocity.y;
		}

		logDebug(ballPosition);

	}

	void render(Window& window) override {

		window.renderer.clearColor(Color(0));

		window.renderer.normalizedSpace();

		//paddle 1
		window.renderer.uniforms().data.mMatrix = 
		scale(translate(Matrix(1),Vec3(-0.9, paddle1Location,0)),Vec3(0.01, 0.2, 0.5));
		
		
		
		
		window.renderer.useShader("white");
		window.renderer.renderMesh("cube");

	}
};