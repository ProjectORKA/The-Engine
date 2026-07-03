#pragma once
#include "Game.hpp"
#include "PongParticleSystem.hpp"
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

	void update(Window& window);
	void mouseInput(DVec2 cursorWorldPosition);
	void ballPositionInput(Vector<Ball>& balls);
	void aiInput(Vector<Ball>& balls, Float deltaTime);
};

struct Ball
{
	Float desiredSpeed   = 2;
	Bool  stuckToPaddle  = true;
	Bool  stuckToPaddle1 = true;
	Vec2  position       = Vec2(0);
	Vec2  velocity       = Vec2(1, 0);

	void update(Float deltaTime, PongPlayer players[2], PongParticleSystem* collisionParticles = nullptr);
	void render(Renderer& renderer) const;
};

struct PongRenderer final : GameRenderer
{
	Vector<Ball>       balls;
	PongPlayer         players[2];
	PongParticleSystem collisionParticles = PongParticleSystem(256);
	UInt               ballCount          = 1;
	InputId      shootButton1secondary = InputId(InputType::Mouse, 0);
	InputId      moveUpButton1         = InputId(InputType::KeyBoard, W);
	InputId      moveDownButton1       = InputId(InputType::KeyBoard, S);
	InputId      moveUpButton2         = InputId(InputType::KeyBoard, Up);
	InputId      moveDownButton2       = InputId(InputType::KeyBoard, Down);
	InputId      shootButton2          = InputId(InputType::KeyBoard, Left);
	InputId      shootButton1          = InputId(InputType::KeyBoard, Space);

	void update(Window& window) override;
	void create(Window& window) override;
	void renderScene(Window& window) override;
	void destroy(Window& window) override;
	void renderInteractive(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;
};

struct Pong
{
	void run()
	{
		PongRenderer renderer1;
		Window & window = ui.window("Pong", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed);
		window.row();
		window.addElement(renderer1);
		ui.run();
	}
};

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls);
