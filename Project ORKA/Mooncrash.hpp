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

struct Mooncrash : public Game {
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

	Time time;
	PlanetSystem planetSystem;

	Mooncrash();
	void update() override;
	void render(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;
};
void renderMooncrashAtmosphere(Renderer& renderer, MooncrashPlayer& player);
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetSystemPlayer& player);