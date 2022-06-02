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
	void render(Window& window) override;
};

struct MooncrashSimulation : public GameSimulation {
	Time time;
	PlanetSystem planetSystem;

	void update(Float timeStep) override;
};

struct MooncrashRenderer : public GameRenderer {

	MooncrashSimulation* simulation = nullptr;

	Vec3 sunDirection = Vec3(0, 0, 1);

	Float mouseSensitivity = 0.0015f;

	MooncrashPlayer player;

	//input
	InputEvent wireframeToggle = InputEvent(InputType::KeyBoard, F, 0);
	InputEvent countNodesButton = InputEvent(InputType::KeyBoard, G, 0);
	InputEvent worldDistortion = InputEvent(InputType::KeyBoard, K, 1);
	InputEvent rebuildShaders = InputEvent(InputType::KeyBoard, R, 0);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);

	MooncrashRenderer(MooncrashSimulation & simulation);
	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void mouseMoved(Window& window, MouseMovementInput input) override;
};

void renderMooncrashAtmosphere(Renderer& renderer, MooncrashPlayer& player);
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player);