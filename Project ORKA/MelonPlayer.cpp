#include "MelonPlayer.hpp"
#include "UserInterface.hpp"
#include "Window.hpp"

MelonPlayer::MelonPlayer()
{
	camera.setFieldOfView(90.0f);
	camera.setNearClipValue(0.125f);
	camera.setFarClipValue(256.0f);

	trail.resize(1000);
	for(Vec3& trailPoint : trail) trailPoint = position;
}

void MelonPlayer::update(Window& window)
{
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

	// update smoke
	smoke.update(position, window.renderer);

	// player rotation
	expDecay(currentVisibleDirection, direction, 8, delta);
	orientation  = Orientation(currentVisibleDirection);
	lastPosition = position;

	// update camera
	const Vec3 cameraTargetPosition = position + Vec3(0.0f, 0.0f, 1.0f + powf(1.2f, static_cast<Float>(zoomFactor)));
	const Vec3 cameraDesiredDelta   = cameraTargetPosition - camera.getPosition();
	camera.setPosition(camera.getPosition() + cameraDesiredDelta * delta * 10.0f);

	for(Int i = static_cast<Int>(trail.size()) - 1; i > 0; --i) trail[i] = trail[i - 1];
	trail[0] = position;
}

void MelonPlayer::inputEvent(Window& window, InputEvent& input)
{
	if(input == zoomIn) zoomFactor++;
	if(input == zoomOut) zoomFactor--;
	zoomFactor = clamp(zoomFactor, -3, 30);

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
	}
}

void MelonPlayer::render(Window& window)
{
	Renderer& renderer = window.renderer;

	camera.setRotation(DVec3(PI / 2, 0, 0));

	camera.render(renderer);

	// render player
	renderer.useShader("MelonUberShader");
	renderer.uniforms().setMMatrix(matrixFromOrientation(orientation, position, 0.5) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().setCustomColor(Vec4(1));
	renderer.renderMesh("melonPlayerRolling");

	Vec3Vector lines;
	lines.reserve((trail.size() - 1) * 2);
	for(Int i = 0; i < static_cast<Int>(trail.size()) - 1; i++)
	{
		lines.emplace_back(trail[i]);
		lines.emplace_back(trail[i + 1]);
	}
	renderer.fill(1,0,0);
	renderer.lines(lines);

	renderer.fill(1);
	// Smoke quads should render double-sided while still respecting scene depth.
	renderer.setCulling(false);
	renderer.setDepthTest(true);
	smoke.render(renderer);
	renderer.setDepthTest(true);
	renderer.setCulling(true);
}
