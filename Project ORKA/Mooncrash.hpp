#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_TERRAIN_LIMIT false

struct MooncrashPlayer : public PlanetSystemPlayer {

	Matrix skyRotation = Matrix(1);

	MooncrashPlayer();
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

	MooncrashPlayer player;

	//input
	InputEvent vertexColorToggle = InputEvent(InputType::KeyBoard, H, 0);
	InputEvent wireframeToggle = InputEvent(InputType::KeyBoard, F, 0);
	InputEvent countNodesButton = InputEvent(InputType::KeyBoard, G, 0);
	InputEvent worldDistortion = InputEvent(InputType::KeyBoard, K, 1);
	InputEvent rebuildShaders = InputEvent(InputType::KeyBoard, R, 0);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);



	void update(Window& window) override;
	void create() {
		player.speedExponent = 200;
	};
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

void renderMooncrashAtmosphere(ResourceManager& resourceManager, Renderer& renderer, MooncrashPlayer& player);
void renderPlanet(ResourceManager& resourceManager, Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player);

struct Mooncrash{
	ResourceManager resourceManager; 
	UserInterface ui;
	MooncrashSimulation sim;
	MooncrashRenderer renderer;

	void run() {
		renderer.simulation = &sim;

		ui.window("Mooncrash", Area(1920, 1080), true, WindowState::fullscreen, renderer, resourceManager);
		ui.run();
	}
};