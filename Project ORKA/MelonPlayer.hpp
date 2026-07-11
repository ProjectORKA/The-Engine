#pragma once

#include "Basics.hpp"
#include "Player.hpp"
#include "ParticleSystem.hpp"

struct Window;

struct MelonPlayer final : Player
{
	static constexpr Float baseSize = 1.0f;
	static constexpr Int   minZoomFactor = -3;
	static constexpr Int   maxZoomFactor = 25;

	struct GhostTrailSample
	{
		Vec3  position      = Vec3(0);
		Float timestampSecs = 0.0f;
	};

	Float          distanceTraveled        = 0;
	Float          distanceInRadians       = 0;
	Float          speedUnitsPerSec        = 0;
	Float          smoothedSpeedUnitsPerSec = 0;
	Float          smoothedAccelerationUnitsPerSec2 = 0;
	Float          turnAmount01            = 0;
	Int            zoomFactor              = maxZoomFactor;
	Float          mouseSensitivity        = 0.0015f;
	Float          teleportDistance        = 100.0f;
	Float          ghostTrailDurationSecs  = 4.0f;
	Float          ghostTimelineSecs       = 0.0f;
	// Higher fall rate lets acceleration return to zero quickly after stopping.
	Float          accelerationSmoothingRiseRate = 18.0f;
	Float          accelerationSmoothingFallRate = 45.0f;
	Vec3           position                = Vec3(0);
	Vec3           targetPosition          = Vec3(0);
	Vec3           velocity                = Vec3(0);
	Vec3           lastPosition            = Vec3(0);
	Vec3           deltaPosition           = Vec3(0);
	Vec3           virtualAudioDeltaPosition = Vec3(0);
	Vec3           direction               = Vec3(0, 1, 0);
	Vec3           currentVisibleDirection = Vec3(0, 1, 0);
	Vec3           lastPositionAtDistance  = Vec3(0, -1, 0);
	Vec3           pendingTeleportDirection = Vec3(0, 1, 0);
	Float          pendingTeleportDistance  = 0.0f;
	ParticleSystem smoke                   = ParticleSystem(1024 * 32);
	Orientation    orientation             = Orientation(Vec3(0, 1, 0));

	// input
	InputEvent zoomIn          = InputEvent(InputType::Scroll, 1, false);
	InputEvent zoomOut         = InputEvent(InputType::Scroll, 1, true);
	InputEvent teleportForward = InputEvent(InputType::KeyBoard, Space, true);

	Deque<GhostTrailSample> ghostTrail;

	MelonPlayer();
	void respawn();
	void update(Window& window) override;
	void inputEvent(Window& window, InputEvent& input) override;
	void render(Window& window) override;
};
