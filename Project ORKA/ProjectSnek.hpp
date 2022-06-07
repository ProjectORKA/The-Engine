#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "Basics.hpp"
#include "Math.hpp"

struct SnakeFood;

struct Snake {
	Vec2 headPosition = Vec2(-1, 0);
	Vec2 direction = Vec2(0, 0);
	Color snakeColor = Color(0.2, 0.5, 0, 1);
	Vector<Vec2> bodySegments;

	void addSegment() {
		bodySegments.push_back(Vec2(0, 0));
	}

	void render(Window& window) {
		window.renderer.fill(Color(snakeColor));
		window.renderer.useShader("color");
		window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(headPosition,0.03));
		window.renderer.renderMesh("circle");
		for (Int i = 0; i < bodySegments.size(); i++) {
			window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(bodySegments[i], 0.03));
			window.renderer.renderMesh("circle");
		}
	}

	void update(SnakeFood& snakefood);
};

struct SnakeFood {
	Vec2 foodPosition = Vec2(-0.5, -0.5);
	Vec2 direction = Vec2(0, 0);
	Color foodColor = Color(0.5, 0.1, 0, 1);

	void render(Window& window) {
		window.renderer.fill(Color(foodColor));
		window.renderer.useShader("color");
		window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(foodPosition, 0.03));
		window.renderer.renderMesh("circle");

	}
	void update() {
		foodPosition += randomVec2(-0.02, 0.02);
		foodColor = randomVec4(0, 1);
		if (foodPosition.x >= 1 || foodPosition.x <= -1 || foodPosition.y >= 1 || foodPosition.y <= -1) {
			foodPosition = randomVec2(-0.9, 0.9);
		}
	}
};

struct SnakeGame :public GameRenderer {
	Snake snake;
	SnakeFood snakefood;
	Float totalTime = 0;
	InputEvent forward = InputEvent(InputType::KeyBoard, W, 1);
	InputEvent left = InputEvent(InputType::KeyBoard, A, 1);
	InputEvent back = InputEvent(InputType::KeyBoard, S, 1);
	InputEvent right = InputEvent(InputType::KeyBoard, D, 1);

	void inputEvent(Window& window, InputEvent input) override {
		if (input == forward) {
			snake.direction = Vec2(0, 0.01);
	}
		if (input == left) {
			snake.direction = Vec2(-0.01, 0);
		}
		if (input == back) {
			snake.direction = Vec2(0, -0.01);
		}
		if (input == right) {
			snake.direction = Vec2(0.01, 0);
		}
	};

	void render(Window& window, TiledRectangle area) override {
		window.renderer.clearColor(Color(0, 0.2, 0.2, 0.7));
		window.renderer.aspectRatioNormalizedSpace(16.0 / 9.0);
		totalTime += window.renderer.deltaTime();
		if (totalTime > 0.016666) {
			snake.update(snakefood);
			snakefood.update();
			totalTime = 0;
		}
		window.renderer.useShader("color");
		window.renderer.renderMesh("centeredPlane");
		snake.render(window);
		snakefood.render(window);


		logDebug(totalTime);
	}
};
