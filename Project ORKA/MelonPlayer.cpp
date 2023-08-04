#include "MelonPlayer.hpp"
#include "Window.hpp"

MelonPlayer::MelonPlayer()
{
	camera.setFieldOfView(90.0f);
	camera.setNearClipValue(0.125f);
	camera.setFarClipValue(256.0f);

	trail.resize(1000);
}

void MelonPlayer::update(Window& window)
{
	// mouse input
	if(window.capturing)
	{
		auto delta = Vec3(Vec2(window.mouseDelta) * Vec2(mouseSensitivity), 0);
		delta *= camera.getLocation().z;
		velocity = Vec3(0);
		targetLocation += delta;
	}
	else
	{
		targetLocation = Vec3(window.mousePosBotLeft, 0);
	}

	// get frequently used info
	const Float delta = window.renderer.deltaTime();

	// update target location
	targetLocation += velocity * delta;

	approach(location, targetLocation, delta * 300);

	// player location (smoothed)
	deltaLocation                            = location - lastLocation;
	lastLocationAtDistance                   = location + 0.01f * normalize(lastLocationAtDistance - location);
	const Vec3 deltaToLastLocationAtDistance = location - lastLocationAtDistance;
	distanceTraveled                         = length(deltaLocation);
	distanceInRadians += distanceTraveled;
	distanceInRadians = mod(distanceInRadians, PI);
	if(distanceTraveled != 0.0f) direction = normalize(deltaToLastLocationAtDistance);

	// update smoke
	smoke.update(location, window.renderer);

	// player rotation
	approach(currentVisibleDirection, direction, delta * 8);
	orientation  = Orientation(currentVisibleDirection);
	lastLocation = location;

	// update camera
	const Vec3 cameraTargetLocation = location + Vec3(0.0f, 0.0f, 1.0f + powf(1.2f, static_cast<Float>(zoomFactor)));
	const Vec3 cameraDesiredDelta   = cameraTargetLocation - camera.getLocation();
	camera.setLocation(camera.getLocation() + cameraDesiredDelta * delta * 10.0f);

	trail[0] = location;

	for(Int i = static_cast<Int>(trail.size() - 1); i > 1; i--) trail[i] = trail[i - 1];
}

void MelonPlayer::inputEvent(Window& window, const InputEvent input)
{
	if(input == zoomIn) zoomFactor++;
	if(input == zoomOut) zoomFactor--;
	zoomFactor = clamp(zoomFactor, -3, 30);
}

void MelonPlayer::render(ResourceManager& resourceManager, Window& window)
{
	Renderer& renderer = window.renderer;

	camera.setRotation(DVec3(PI / 2, 0, 0));

	camera.render(renderer);

	// render player
	renderer.useShader(resourceManager, "MelonUberShader");
	renderer.uniforms().setMMatrix(matrixFromOrientation(orientation, location, 0.5) * rotate(Matrix(1), -2 * distanceInRadians, Vec3(1, 0, 0)));
	renderer.uniforms().setCustomColor(Vec4(1));
	renderer.renderMesh(resourceManager, "melonPlayerRolling");

	Vector<Line3D> lines;
	for(Int i = 0; i < static_cast<Int>(trail.size()) - 1; i++) lines.push_back(Line3D(trail[i], trail[i + 1]));
	renderer.lines(lines);

	smoke.render(resourceManager, renderer);
}
