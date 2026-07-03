#pragma once
#include "Game.hpp"
#include "PlanetRenderSystem.hpp"
#include "PlanetSystem.hpp"
#include "PlanetSystemPlayer.hpp"

constexpr auto initialCameraSpeed = 200; // 1 as fast as a human 400 as fast as light;
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
	Float                mouseSensitivity  = 0.0015f;
	MoonCrashSimulation* simulation        = nullptr;
	Vec3                 sunDirection      = Vec3(0.0f, 0.0f, 1.0f);
	InputEvent           enter             = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent           worldDistortion   = InputEvent(InputType::KeyBoard, K, true);
	InputEvent           exit              = InputEvent(InputType::Mouse, Rmb, false);
	InputEvent           vertexColorToggle = InputEvent(InputType::KeyBoard, H, false);
	InputEvent           chunkBoundsToggle = InputEvent(InputType::KeyBoard, B, false);
	InputEvent           wireFrameToggle   = InputEvent(InputType::KeyBoard, F, false);
	InputEvent           countNodesButton  = InputEvent(InputType::KeyBoard, G, false);
	InputEvent           rebuildShaders    = InputEvent(InputType::KeyBoard, R, false);

	PlanetRenderSystem planetRenderSystem;

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void create(Window& window) override;
	void renderScene(Window& window) override;

	void renderInteractive(Window& window) override {}

	void renderPlanet(Renderer& r);
	void renderMoonCrashAtmosphere(Renderer& r) const;
};

struct MoonCrash
{
	MoonCrashSimulation sim;
	MoonCrashRenderer   renderer;

	void run()
	{
		sim.start();
		while (!sim.isLoaded()) {}
		renderer.connect(sim);
		ui.window("MoonCrash", settings.defaultWindowWidth, settings.defaultWindowHeight, true, true, WindowState::Windowed, renderer);
		ui.run();
		sim.stop();
	}
};
