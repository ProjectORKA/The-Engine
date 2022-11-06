
//written by deg
//modified by Blendurian

#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "Basics.hpp"
#include "Math.hpp"

#include "Velox_Vector.hpp"

struct SnakeFood;

struct Snake {
	Float speed = 1;
	Float segmentRadius = 0.03;
	Vec2 headPosition = Vec2(-1, 0);
	Vec2 direction = Vec2(0, 0);
	Color snakeColor = Color(0.2, 0.5, 0, 1);
	Vector<Vec2> bodySegments;

	InputEvent forward = InputEvent(InputType::KeyBoard, W, 1);
	InputEvent left = InputEvent(InputType::KeyBoard, A, 1);
	InputEvent back = InputEvent(InputType::KeyBoard, S, 1);
	InputEvent right = InputEvent(InputType::KeyBoard, D, 1);

	void addSegment();
	void dumbAI(SnakeFood& f);
	void render(Window& window);
	void inputEvent(Window& window, InputEvent input);
	void update(SnakeFood& snakefood, Float deltaTime);
};

struct SnakeFood {
	Vec2 foodPosition = Vec2(-0.5, -0.5);
	Vec2 direction = Vec2(0, 0);
	Color foodColor = Color(0.5, 0.1, 0, 1);

	void update();
	void render(Window& window);
};

struct SnakeGame :public GameRenderer {
	Snake snake;
	SnakeFood snakefood;
	Float totalTime = 0;

	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
};
