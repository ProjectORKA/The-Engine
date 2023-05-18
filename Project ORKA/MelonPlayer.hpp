#pragma once

#include "Basics.hpp"
#include "Player.hpp"
#include "ParticleSystem.hpp"

struct Window;

struct MelonPlayer : public Player {
	Int zoomFactor = 30;
	Vec3 location = Vec3(0);
	Vec3 targetLocation = Vec3(0);
	Vec3 velocity = Vec3(0);
	Float distanceTraveled = 0;
	Vec3 lastLocation = Vec3(0);
	Float distanceInRadians = 0;
	Vec3 deltaLocation = Vec3(0);
	Vec3 direction = Vec3(0, 1, 0);
	Float mouseSensitivity = 0.0015;
	Vec3 lastLocationAtDistance = Vec3(0, -1, 0);
	Vec3 currentVisibleDirection = Vec3(0, 1, 0);
	ParticleSystem smoke = ParticleSystem(1024 * 32);
	Orientation orientation = Orientation(Vec3(0, 1, 0));

	//input
	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, false);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, true);

	MelonPlayer();
	void update(Window& window) override;
	void render(ResourceManager& resourceManager, Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
};
