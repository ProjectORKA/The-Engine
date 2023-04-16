
#include "Player.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

void Player::update(Window& window) {
	//get frequently used info
	Float delta = window.renderer.deltaTime();

	//set up temporary data
	Vec3 movementVector = Vec3(0);
	Float desiredSpeed = 1;

	//process input
	if (window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if (window.pressed(forward)) movementVector += camera.forwardVector;
	if (window.pressed(backward)) movementVector -= camera.forwardVector;
	if (window.pressed(right)) movementVector += camera.rightVector;
	if (window.pressed(left)) movementVector -= camera.rightVector;
	if (window.pressed(up)) movementVector += camera.upVector;
	if (window.pressed(down)) movementVector -= camera.upVector;

	//calculate movement
	if (length(movementVector) > 0) {					//if there is movement input
		movementVector = normalize(movementVector);		//get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;			//add speed to direction
		camera.location += movementVector;				//add it to cameras location
	}
}

void Player::render(ResourceManager& resourceManager, Window & window) {
	camera.render(window.renderer);		//set up matrices to view the world from cameras perspective
}

void DebugPlayer::update(Window & window) {
	//get frequently used info
	Float delta = window.renderer.deltaTime();
	
	//set up temporary data
	Vec3 movementVector = Vec3(0);
	Float desiredSpeed = 0;

	//process input
	if (window.capturing) camera.rotate(window.mouseDelta * MouseMovement(mouseSensitivity));
	if (window.pressed(forward)) movementVector += camera.forwardVector;
	if (window.pressed(backward)) movementVector -= camera.forwardVector;
	if (window.pressed(right)) movementVector += camera.rightVector;
	if (window.pressed(left)) movementVector -= camera.rightVector;
	if (window.pressed(up)) movementVector += camera.upVector;
	if (window.pressed(down)) movementVector -= camera.upVector;

	//calculate movement
	if (length(movementVector) > 0) {					//if there is movement input
		desiredSpeed = pow(baseNumber, speedExponent);	//calculate speed
		movementVector = normalize(movementVector);		//get direction of movement (just direction)
		movementVector *= desiredSpeed * delta;			//add speed to direction
		camera.location += movementVector;				//add it to cameras location
	}
}

void DebugPlayer::inputEvent(Window& window, InputEvent input) {
	if (input == faster) speedExponent++;
	if (input == slower) speedExponent--;
}
