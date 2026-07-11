#include "MelonPlayer.hpp"
#include "UserInterface.hpp"
#include "Window.hpp"

MelonPlayer::MelonPlayer()
{
	camera.setFieldOfView(90.0f);
	camera.setNearClipValue(0.125f);
	camera.setFarClipValue(256.0f);
	ghostTrail.push_back({position, ghostTimelineSecs});
}

void MelonPlayer::respawn()
{
	distanceTraveled                = 0.0f;
	distanceInRadians               = 0.0f;
	speedUnitsPerSec                = 0.0f;
	smoothedSpeedUnitsPerSec        = 0.0f;
	smoothedAccelerationUnitsPerSec2 = 0.0f;
	turnAmount01                    = 0.0f;
	zoomFactor                      = maxZoomFactor;
	ghostTimelineSecs               = 0.0f;

	position                 = Vec3(0);
	targetPosition           = Vec3(0);
	velocity                 = Vec3(0);
	lastPosition             = Vec3(0);
	deltaPosition            = Vec3(0);
	virtualAudioDeltaPosition = Vec3(0);
	direction                = Vec3(0, 1, 0);
	currentVisibleDirection  = Vec3(0, 1, 0);
	lastPositionAtDistance   = Vec3(0, -1, 0);
	pendingTeleportDirection = Vec3(0, 1, 0);
	pendingTeleportDistance  = 0.0f;
	orientation              = Orientation(Vec3(0, 1, 0));

	camera.setPosition(Vec3(0));
	camera.setRotation(DVec3(0));
	camera.setFieldOfView(90.0f);
	camera.setNearClipValue(0.125f);
	camera.setFarClipValue(256.0f);

	ghostTrail.clear();
	ghostTrail.push_back({position, ghostTimelineSecs});

	smoke.currentId              = 0;
	smoke.distanceSinceLastSpawn = 0.0f;
	smoke.position               = Vec3(0);
	smoke.hasOlderSample         = false;
	smoke.hasNewerSample         = false;
	smoke.olderSamplePos         = Vec3(0);
	smoke.newerSamplePos         = Vec3(0);
	smoke.olderSampleTime        = 0.0;
	smoke.newerSampleTime        = 0.0;
	for(UInt i = 0; i < smoke.particleCount; ++i)
	{
		smoke.alive[i]          = false;
		smoke.lifetime[i]       = 0.0f;
		smoke.maxLifetime[i]    = 1.0f;
		smoke.maxSize[i]        = 0.0f;
		smoke.spawnPosition[i]  = Vec3(0);
		smoke.spawnVelocity[i]  = Vec3(0);
		smoke.transformation[i] = Vec4(0);
	}
}

void MelonPlayer::update(Window& window)
{
	const Vec3 previousDirection = direction;
	const Float previousSmoothedSpeed = smoothedSpeedUnitsPerSec;

	// mouse input
	if(ui.capturing)
	{
		auto delta = Vec3(Vec2(window.mouseDelta) * Vec2(mouseSensitivity), 0);
		delta *= camera.getPosition().z;
		velocity = Vec3(0);
		targetPosition += delta;
	}

	// get frequently used info
	const Float delta = window.renderer.deltaTime();
	ghostTimelineSecs += delta;

	// update target position
	targetPosition += velocity * delta;

	expDecay(position, targetPosition, 300, delta);

	// player position (smoothed)
	deltaPosition                            = position - lastPosition;
	lastPositionAtDistance                   = position + 0.01f * normalize(lastPositionAtDistance - position);
	const Vec3 deltaToLastPositionAtDistance = position - lastPositionAtDistance;
	distanceTraveled                         = length(deltaPosition);
	distanceInRadians += distanceTraveled;
	distanceInRadians = mod(distanceInRadians, static_cast<Float>(PI));
	if(distanceTraveled != 0.0f) direction = normalize(deltaToLastPositionAtDistance);

	const Float safeDelta = max(delta, 0.0001f);
	const Float teleportSpeedUnitsPerSec = pendingTeleportDistance / safeDelta;
	virtualAudioDeltaPosition = deltaPosition + pendingTeleportDirection * pendingTeleportDistance;
	speedUnitsPerSec      = distanceTraveled / safeDelta + teleportSpeedUnitsPerSec;
	expDecay(smoothedSpeedUnitsPerSec, speedUnitsPerSec, 10.0f, safeDelta);

	const Float rawAcceleration = (smoothedSpeedUnitsPerSec - previousSmoothedSpeed) / safeDelta;
	const Float accelerationDecay = abs(rawAcceleration) >= abs(smoothedAccelerationUnitsPerSec2) ? accelerationSmoothingRiseRate : accelerationSmoothingFallRate;
	expDecay(smoothedAccelerationUnitsPerSec2, rawAcceleration, accelerationDecay, safeDelta);
	pendingTeleportDistance = 0.0f;

	turnAmount01 = 0.0f;
	if(length(previousDirection) > 0.0001f && length(direction) > 0.0001f)
	{
		const Float directionalAlignment = clamp(dot(normalize(previousDirection), normalize(direction)), -1.0f, 1.0f);
		turnAmount01 = 1.0f - directionalAlignment;
	}

	// update smoke
	smoke.update(position, window.renderer);

	// player rotation
	expDecay(currentVisibleDirection, direction, 8, delta);
	orientation  = Orientation(currentVisibleDirection);
	lastPosition = position;

	ghostTrail.push_back({position, ghostTimelineSecs});
	const Float minTimeToKeep = ghostTimelineSecs - max(ghostTrailDurationSecs, 0.0f);
	while(ghostTrail.size() > 1 && ghostTrail.front().timestampSecs < minTimeToKeep) ghostTrail.pop_front();

	// update camera
	const Vec3 cameraTargetPosition = position + Vec3(0.0f, 0.0f, 1.0f + powf(1.2f, static_cast<Float>(zoomFactor)));
	const Vec3 cameraDesiredDelta   = cameraTargetPosition - camera.getPosition();
	camera.setPosition(camera.getPosition() + cameraDesiredDelta * delta * 10.0f);

}

void MelonPlayer::inputEvent(Window& window, InputEvent& input)
{
	if(input == zoomIn) zoomFactor++;
	if(input == zoomOut) zoomFactor--;
	zoomFactor = clamp(zoomFactor, minZoomFactor, maxZoomFactor);

	if(input == teleportForward)
	{
		Vec3 forward = direction;
		if(length(forward) == 0.0f) forward = Vec3(0, 1, 0);
		forward = normalize(forward);

		const Vec3 teleportDelta = forward * teleportDistance;
		position += teleportDelta;
		targetPosition += teleportDelta;
		lastPosition += teleportDelta;
		lastPositionAtDistance += teleportDelta;
		pendingTeleportDirection = forward;
		pendingTeleportDistance += teleportDistance;
	}
}

void MelonPlayer::render(Window& window)
{
	Renderer& renderer = window.renderer;

	camera.setRotation(DVec3(PI / 2, 0, 0));

	camera.render(renderer);

	// render player
	renderer.useShader("MelonUberShader");
	renderer.uniforms().setMMatrix(matrixFromOrientation(orientation, position, baseSize) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().setCustomColor(Vec4(1));
	renderer.renderMesh("melonPlayerRolling");

	Vec3Vector trailSegments;
	if(ghostTrail.size() >= 2)
	{
		const Float trailHeightOffset = 0.03f;
		trailSegments.reserve((ghostTrail.size() - 1) * 2);
		for(SizeT i = 0; i < ghostTrail.size() - 1; ++i)
		{
			Vec3 a = ghostTrail[i].position;
			Vec3 b = ghostTrail[i + 1].position;
			a.z += trailHeightOffset;
			b.z += trailHeightOffset;
			trailSegments.emplace_back(a);
			trailSegments.emplace_back(b);
		}

		renderer.useShader("color");
		renderer.uniforms().setMMatrix(Matrix(1));
		renderer.fill(1, 0, 0);
		renderer.lines(trailSegments);
	}

	if(!ghostTrail.empty())
	{
		Vec3 ghostDirection = currentVisibleDirection;
		if(ghostTrail.size() >= 2)
		{
			const Vec3 ghostPathDirection = ghostTrail[1].position - ghostTrail[0].position;
			if(length(ghostPathDirection) > 0.0001f) ghostDirection = normalize(ghostPathDirection);
		}

		renderer.useShader("MelonUberShader");
		renderer.uniforms().setMMatrix(matrixFromOrientation(Orientation(ghostDirection), ghostTrail.front().position, baseSize) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
		renderer.uniforms().setCustomColor(Vec4(1, 0, 0, 1));
		renderer.renderMesh("melonPlayerRolling");
	}

	renderer.fill(1);
	// Smoke quads should render double-sided while still respecting scene depth.
	renderer.setCulling(false);
	renderer.setDepthTest(true);
	smoke.render(renderer);
	renderer.setDepthTest(true);
	renderer.setCulling(true);
}
