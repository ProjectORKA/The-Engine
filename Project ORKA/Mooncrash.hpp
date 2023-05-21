#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

constexpr auto initialCameraSpeed = 200;				//1 as fast as a human 400 as fast as light;
#define CAMERA_TERRAIN_LIMIT false

struct MoonCrashPlayer : public PlanetSystemPlayer
{
	Matrix skyRotation = Matrix(1);

	MoonCrashPlayer();
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window) override;
};

struct MoonCrashSimulation final : public GameSimulation
{
	Time time;
	PlanetSystem planetSystem;

	void update(Float delta) override;
	void create(ResourceManager& resourceManager) override;
};

struct MoonCrashRenderer final : public GameRenderer
{
	MoonCrashPlayer player;
	Float mouseSensitivity = 0.0015f;
	Vec3 sunDirection = Vec3(0.0f, 0.0f, 1.0f);
	MoonCrashSimulation* simulation = nullptr;
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);
	InputEvent vertexColorToggle = InputEvent(InputType::KeyBoard, H, false);
	InputEvent wireFrameToggle = InputEvent(InputType::KeyBoard, F, false);
	InputEvent countNodesButton = InputEvent(InputType::KeyBoard, G, false);
	InputEvent worldDistortion = InputEvent(InputType::KeyBoard, K, true);
	InputEvent rebuildShaders = InputEvent(InputType::KeyBoard, R, false);

	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

void renderMoonCrashAtmosphere(ResourceManager& resourceManager, Renderer& renderer, MoonCrashPlayer& player);
void renderPlanet(ResourceManager& resourceManager, Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player);

struct MoonCrash
{
	ResourceManager resourceManager;
	UserInterface ui;
	MoonCrashSimulation sim;
	MoonCrashRenderer renderer;

	void run();
};