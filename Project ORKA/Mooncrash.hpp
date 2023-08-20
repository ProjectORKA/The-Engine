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
	void render(ResourceManager& resourceManager, Window& window) override;
};

struct MoonCrashSimulation final : GameSimulation
{
	Time         time;
	PlanetSystem planetSystem;

	void destroy() override;
	void update(Float delta) override;
	void create(ResourceManager& resourceManager) override;
};

struct MoonCrashRenderer final : GameRenderer
{
	MoonCrashPlayer      player;
	Framebuffer          mainFramebuffer;
	Float                mouseSensitivity  = 0.0015f;
	MoonCrashSimulation* simulation        = nullptr;
	Vec3                 sunDirection      = Vec3(0.0f, 0.0f, 1.0f);
	InputEvent           enter             = InputEvent(InputType::Mouse, LMB, true);
	InputEvent           worldDistortion   = InputEvent(InputType::KeyBoard, K, true);
	InputEvent           exit              = InputEvent(InputType::Mouse, RMB, false);
	InputEvent           vertexColorToggle = InputEvent(InputType::KeyBoard, H, false);
	InputEvent           wireFrameToggle   = InputEvent(InputType::KeyBoard, F, false);
	InputEvent           countNodesButton  = InputEvent(InputType::KeyBoard, G, false);
	InputEvent           rebuildShaders    = InputEvent(InputType::KeyBoard, R, false);

	void update(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;

	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override {}
};

void renderMoonCrashAtmosphere(ResourceManager& resourceManager, Renderer& renderer, MoonCrashPlayer& player, Framebuffer& framebuffer);
void renderPlanet(ResourceManager& resourceManager, Renderer& renderer, PlanetSystem& planetSystem, const PlanetSystemPlayer& player, const Framebuffer& framebuffer);

struct MoonCrash
{
	UserInterface       ui;
	MoonCrashSimulation sim;
	MoonCrashRenderer   renderer;
	ResourceManager     resourceManager;

	void run();
};
