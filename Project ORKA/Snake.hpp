// written by deg
// modified by Blendurian

#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Window.hpp"

struct SnakeFood;

struct Snake
{
	Float        speed         = 1.0f;
	Float        segmentRadius = 0.03f;
	Vec2         direction     = Vec2(0, 0);
	Vec2         headPosition  = Vec2(-1, 0);
	Color        snakeColor    = Color(0.2, 0.5, 0, 1);
	Vector<Vec2> bodySegments;

	InputEvent forward = InputEvent(InputType::KeyBoard, W, true);
	InputEvent left    = InputEvent(InputType::KeyBoard, A, true);
	InputEvent back    = InputEvent(InputType::KeyBoard, S, true);
	InputEvent right   = InputEvent(InputType::KeyBoard, D, true);

	void addSegment();
	void dumbAI(const SnakeFood& f);
	void inputEvent(Window& window, InputEvent input);
	void update(SnakeFood& snakefood, Float deltaTime);
	void render(ResourceManager& resourceManager, Window& window);
};

struct SnakeFood
{
	Vec2  direction    = Vec2(0, 0);
	Vec2  foodPosition = Vec2(-0.5, -0.5);
	Color foodColor    = Color(0.5, 0.1, 0, 1);

	void update();
	void render(ResourceManager& resourceManager, Window& window);
};

struct SnakeRenderer : GameRenderer
{
	Snake     snake;
	SnakeFood snakefood;
	Float     totalTime = 0;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct SnakeGame
{
	UserInterface   ui;
	SnakeRenderer   renderer;
	ResourceManager resourceManager;

	void run();
};
