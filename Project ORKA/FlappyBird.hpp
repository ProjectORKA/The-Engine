#pragma once

#include "Basics.hpp"
#include "Game.hpp"
#include "Transform.hpp"
#include "Camera.hpp"

struct FlappyBirdPipes
{
	Float     maxGap     = 1.0f;
	Float     minGap     = 0.6f;
	Float     width      = 0.3f;
	Float     spacing    = 1.5f;
	Float     gap        = 0.6f;
	Float     size       = 0.1f;
	Float     speed      = 1.0f;
	Float     posX       = 0.0f;
	Float     posY       = -0.5f;
	Transform transform1 = Transform(Vec3(posX, posY - gap / 2.0f, 0.0f), Vec3(0.0f), Vec3(size));
	Transform transform2 = Transform(Vec3(posX, posY + gap / 2.0f, 0.0f), Vec3(0.0f, 0.0f, 180.0f), Vec3(size));

	void     randomize();
	void     update(Float delta);
	explicit FlappyBirdPipes(Int id);
	void     render(Renderer& r) const;
};

struct FlappyBirdBird
{
	Transform transform   = Vec3(0.0, 0.5f, 0.0);
	Bool      isColliding = false;
	Float     size        = 0.05f;
	Bool      alive       = true;
	Bool      heldInPlace = true;
	Vec3      velocity    = Vec3(0);

	InputEvent jumpButton = InputEvent(InputType::KeyBoard, SPACE, true);

	void jump();
	void reset();
	void inputEvent(InputEvent input);
	void render(Renderer& r) const;
	void update(const Vector<FlappyBirdPipes>& pipeColumns, Float delta);
	void updateAI(const Vector<FlappyBirdPipes>& pipeColumns, Float delta);

	[[nodiscard]] FlappyBirdPipes getClosestPipeColumn(const Vector<FlappyBirdPipes>& pipeColumns) const;
};

struct FlappyBirdRenderer final : GameRenderer
{
	const Int   flappyBirdCount = 1;
	const Bool  enableAI        = false;
	const Float gameSpeed       = 1.0f;
	const Float slowMotionSpeed = 0.5f;

	Vector<FlappyBirdBird>  birds;
	Camera                  camera;
	Vector<FlappyBirdPipes> pipeColumns;

	InputEvent reloadShaders   = InputEvent(InputType::KeyBoard, T, true);
	InputEvent toggleWireFrame = InputEvent(InputType::KeyBoard, F, true);

	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct FlappyBird
{
	UserInterface      ui;
	Window             window;
	FlappyBirdRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("ORKA Flappy Bird", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
