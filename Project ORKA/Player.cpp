
#include "Player.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"

void Player::update(Float delta) {
	accelerationVector *= speed * delta;
	camera.location += accelerationVector;
	accelerationVector = { 0,0,0 };
}

void Player::render(Renderer& renderer) {
	camera.render(renderer);
}
