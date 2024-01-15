#pragma once

#include "Basics.hpp"
#include "Player.hpp"
#include "ParticleSystem.hpp"

struct Window;

struct MelonPlayer : Player
{
	Float          distanceTraveled        = 0;
	Float          distanceInRadians       = 0;
	Int            zoomFactor              = 30;
	Float          mouseSensitivity        = 0.0015f;
	Vec3           location                = Vec3(0);
	Vec3           targetLocation          = Vec3(0);
	Vec3           velocity                = Vec3(0);
	Vec3           lastLocation            = Vec3(0);
	Vec3           deltaLocation           = Vec3(0);
	Vec3           direction               = Vec3(0, 1, 0);
	Vec3           currentVisibleDirection = Vec3(0, 1, 0);
	Vec3           lastLocationAtDistance  = Vec3(0, -1, 0);
	ParticleSystem smoke                   = ParticleSystem(1024 * 32);
	Orientation    orientation             = Orientation(Vec3(0, 1, 0));

	// input
	InputEvent zoomIn  = InputEvent(InputType::Scroll, 1, false);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, true);

	//trail
	Vector<Vec3> trail;

	MelonPlayer();
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void render(Window& window) override;
};
