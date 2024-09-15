#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "KeyMap.hpp"

struct FunctionFinderRenderer final : GameRenderer
{
	// math
	Vec2Vector data;
	Vec2Vector best;
	Float      learnRate      = 10;
	Bool       threading      = true;
	Float      bestA          = 5.8;
	Float      bestB          = -1.6f;
	Float      bestC          = 3.1f;
	Float      bestD          = 0.0f;
	Bool       render2D       = true;
	Bool       disableUi      = false;
	Vec4       functionBounds = Vec4(0);
	Float      bestScore      = std::numeric_limits<float>::lowest();

	Mutex       mutex;
	DebugPlayer player;
	Framebuffer framebuffer;
	Float       mouseSensitivity = 0.0015f;
	InputEvent  enter            = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent  wireframeToggle  = InputEvent(InputType::KeyBoard, F, true);

	static Float function1(const Float x, const Float a)
	{
		return a * x;
	}

	static Float function2(const Float x, const Float a, const Float b)
	{
		return x * x * a + x * b;
	}

	static Float function3(const Float x, const Float a, const Float b, const Float c)
	{
		return 5.0f / pow((1 + pow((x / a), b)), c);
	}

	static Float function4(const Float x, const Float a, const Float b, const Float c, const Float d)
	{
		return 50.0f / pow((1 + pow((x / a), b)), c);
	}

	static Vec2 bezier2D(Vec2 start, Vec2 end, Float middle, Float t)
	{
		Vec2 a = start;
		Vec2 b = Vec2(middle, start.y);
		Vec2 c = Vec2(middle, end.y);
		Vec2 d = end;

		Vec2 e = lerp(a, b, t);
		Vec2 f = lerp(b, c, t);
		Vec2 g = lerp(c, d, t);

		Vec2 h = lerp(e, f, t);
		Vec2 i = lerp(f, g, t);

		return lerp(h, i, t);
	}

	[[nodiscard]] Matrix plotMatrix() const;
	void                 create(Window& window) override;
	void                 update(Window& window) override;
	void                 destroy(Window& window) override;
	void                 connect(GameSimulation& simulation) override;
	void                 render(Window& window, TiledRectangle area) override;
	void                 inputEvent(Window& window, InputEvent input) override;
	void                 renderInteractive(Window& window, TiledRectangle area) override;
};

struct FunctionFinder
{
	UserInterface          ui;
	Window                 window;
	FunctionFinderRenderer renderer;

	void run()
	{
		ui.create();
		ui.window("FunctionFinder", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer);
		ui.run();
	}
};
