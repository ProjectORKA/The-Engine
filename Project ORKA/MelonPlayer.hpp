
#pragma once

#include "Basics.hpp"
#include "Player.hpp"
#include "ParticleSystem.hpp"

struct Renderer;

struct MelonPlayer : public Player {
	Int zoomFactor = 10;
	Vec3 location = Vec3(0);
	Vec3 velocity = Vec3(0);
	Float distanceTraveled = 0;
	Vec3 lastLocation = Vec3(0);
	Float distanceInRadians = 0;
	Vec3 deltaLocation = Vec3(0);
	Vec3 direction = Vec3(0, 1, 0);
	Vec3 currentCameraVelocity = Vec3(0);
	Vec3 lastLocationAtDistance = Vec3(0);
	Vec3 currentVisibleDirection = Vec3(0, 1, 0);
	ParticleSystem smoke = ParticleSystem(&location);

	MelonPlayer();
	void update() override;
	void render(Renderer& renderer) override;
};