#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

constexpr auto initialCameraSpeed = 200;				// 1 as fast as a human 400 as fast as light;
#define CAMERA_TERRAIN_LIMIT false

struct MoonCrashPlayer final : PlanetSystemPlayer
{
	Matrix skyRotation = Matrix(1);

	MoonCrashPlayer();
	void update(Window& window) override;
	void render(Window& window) override;
};

struct MoonCrashSimulation final : GameSimulation
{
	Time         time;
	PlanetSystem planetSystem;

	void create() override;
	void destroy() override;
	void update(Float delta) override;
};

struct MoonCrashRenderer final : GameRenderer
{
	MoonCrashPlayer      player;
	Framebuffer          mainFramebuffer;
	Float                mouseSensitivity  = 0.0015f;
	MoonCrashSimulation* simulation        = nullptr;
	Vec3                 sunDirection      = Vec3(0.0f, 0.0f, 1.0f);
	InputEvent           enter             = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent           worldDistortion   = InputEvent(InputType::KeyBoard, K, true);
	InputEvent           exit              = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent           vertexColorToggle = InputEvent(InputType::KeyBoard, H, false);
	InputEvent           wireFrameToggle   = InputEvent(InputType::KeyBoard, F, false);
	InputEvent           countNodesButton  = InputEvent(InputType::KeyBoard, G, false);
	InputEvent           rebuildShaders    = InputEvent(InputType::KeyBoard, R, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(Window& window) override;
	void render(Window& window, TiledRectangle area) override;

	void renderInteractive(Window& window, TiledRectangle area) override {}
};

void renderMoonCrashAtmosphere(Renderer& renderer, MoonCrashPlayer& player, Framebuffer& framebuffer);
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, const PlanetSystemPlayer& player, const Framebuffer& framebuffer);

struct MoonCrash
{
	UserInterface       ui;
	MoonCrashSimulation sim;
	Window              window;
	MoonCrashRenderer   renderer;

	void run()
	{
		ui.create();
		sim.start();
		renderer.connect(sim);
		ui.window("MoonCrash", Area(1920, 1080), true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};
