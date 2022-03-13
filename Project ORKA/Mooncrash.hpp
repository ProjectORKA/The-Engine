#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_TERRAIN_LIMIT false

struct MooncrashPlayer : public PlanetSystemPlayer {
	MooncrashPlayer() {
		chunkLocation.z = ULLONG_MAX / 2;
	}
	void render(Renderer& renderer, Matrix& skyRotation);
};

struct MooncrashSimulation : public GameSimulation {
	Time time;
	PlanetSystem planetSystem;

	void update()override;
};

struct MooncrashRenderer : public GameRenderer {

	MooncrashSimulation* simulation = nullptr;

	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;

	Matrix skyRotationMatrix = Matrix(1);
	Vec3 sunDirection = Vec3(0, 0, 1);

	Float mouseSensitivity = 0.0015f;

	MooncrashPlayer player;

	MooncrashRenderer(MooncrashSimulation & simulation);
	void mouseIsMoving(Window& window, IVec2 position) override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
};
void renderMooncrashAtmosphere(Renderer& renderer, MooncrashPlayer& player);
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player);