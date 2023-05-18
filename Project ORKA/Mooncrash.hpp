#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_TERRAIN_LIMIT false

struct MoonCrashPlayer : public PlanetSystemPlayer {
	Matrix skyRotation = Matrix(1);

	MoonCrashPlayer();
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window) override;
};

struct MooncrashSimulation : public GameSimulation {
	Time time;
	PlanetSystem planetSystem;
	void update(Float delta) override;
	void create(ResourceManager& resourceManager) override;
};

struct MooncrashRenderer : public GameRenderer {
	MooncrashSimulation* simulation = nullptr;

	Vec3 sunDirection = Vec3(0, 0, 1);

	Float mouseSensitivity = 0.0015f;

	MoonCrashPlayer player;

	//input
	InputEvent vertexColorToggle = InputEvent(InputType::KeyBoard, H, false);
	InputEvent wireFrameToggle = InputEvent(InputType::KeyBoard, F, false);
	InputEvent countNodesButton = InputEvent(InputType::KeyBoard, G, false);
	InputEvent worldDistortion = InputEvent(InputType::KeyBoard, K, true);
	InputEvent rebuildShaders = InputEvent(InputType::KeyBoard, R, false);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, false);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, true);


	void update(Window& window) override;
	void create() { player.speedExponent = 200; };
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

void renderMoonCrashAtmosphere(ResourceManager& resourceManager, Renderer& renderer, MoonCrashPlayer& player);
void renderPlanet(ResourceManager& resourceManager, Renderer& renderer, PlanetSystem& planetSystem,
                  PlanetSystemPlayer& player);

struct MoonCrash {
	ResourceManager resourceManager;
	UserInterface ui;
	MooncrashSimulation sim;
	MooncrashRenderer renderer;

	void run() {
		renderer.simulation = &sim;

		ui.window("MoonCrash", Area(1920, 1080), true, WindowState::fullscreen, renderer, resourceManager);
		ui.run();
	}
};
