#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"
#include "PlanetSystemPlayer.hpp"
#include "PlanetSystem.hpp"

#define INITIAL_CAMERA_SPEED 200				//1 as fast as a human 400 as fast as light
#define CAMERA_TERRAIN_LIMIT false

	struct MooncrashPlayer : public PlanetSystemPlayer {
		Int speedMultiplier = INITIAL_CAMERA_SPEED;
		void render(Renderer& renderer) override;
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

		Float mouseSensitivity = 0.0015f;

		MooncrashPlayer player;

		Time time;
		PlanetSystem planetSystem;

		Mooncrash() {
			player.speedMultiplier = 200;
		}

		void update() override;
		void render(Renderer& renderer) override;

		//input
		void mouseIsMoving(Window& window, IVec2 position) override;
		void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
		void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
		void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;;
	};