#include "Player.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

void Player::update(Window& window)
{
	// get frequently used info
	const Float delta = window.renderer.deltaTime();

	// set up temporary data
	auto            movementVector = Vec3(0.0f);
	constexpr Float desiredSpeed   = 1.0f;

	// process input
	if(window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if(window.pressed(forward)) movementVector += camera.getForwardVector();
	if(window.pressed(backward)) movementVector -= camera.getForwardVector();
	if(window.pressed(right)) movementVector += camera.getRightVector();
	if(window.pressed(left)) movementVector -= camera.getRightVector();
	if(window.pressed(up)) movementVector += camera.getUpVector();
	if(window.pressed(down)) movementVector -= camera.getUpVector();

	// calculate movement
	if(length(movementVector) > 0)
	{
		// if there is movement input
		movementVector = normalize(movementVector);					// get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;						// add speed to direction
		camera.setLocation(camera.getLocation() + movementVector);	// add it to cameras location
	}
}

void DebugPlayer::update(Window& window)
{
	// get frequently used info
	const Float delta = window.renderer.deltaTime();

	// set up temporary data
	auto movementVector = Vec3(0);

	// process input
	if(window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if(window.pressed(forward)) movementVector += camera.getForwardVector();
	if(window.pressed(backward)) movementVector -= camera.getForwardVector();
	if(window.pressed(right)) movementVector += camera.getRightVector();
	if(window.pressed(left)) movementVector -= camera.getRightVector();
	if(window.pressed(up)) movementVector += camera.getUpVector();
	if(window.pressed(down)) movementVector -= camera.getUpVector();

	// calculate movement
	if(length(movementVector) > 0)
	{
		Float desiredSpeed = 0.0f;
		// if there is movement input
		desiredSpeed   = powf(baseNumber, static_cast<Float>(speedExponent));	// calculate speed
		movementVector = normalize(movementVector);								// get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;									// add speed to direction
		camera.setLocation(camera.getLocation() + movementVector);				// add it to cameras location
	}
}

void Player::inputEvent(Window& window, InputEvent input) {}

void DebugPlayer::inputEvent(Window& window, const InputEvent input)
{
	if(input == faster) speedExponent++;
	if(input == slower) speedExponent--;
}

void Player::render(ResourceManager& resourceManager, Window& window)
{
	camera.render(window.renderer); // set up matrices to view the world from cameras perspective
}
