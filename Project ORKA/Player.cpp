
#include "Player.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"

void Player::render(Renderer& renderer) {
	speed = pow(1.2f, speedExponent);
	accelerationVector *= speed * renderer.time.delta;
	camera.location += accelerationVector;
	accelerationVector = { 0,0,0 };

	camera.render(renderer);
}
