#include "PongPlayer.hpp"
#include "PongBall.hpp"
#include "Random.hpp"

void PongPlayer::update(Window& window)
{
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void PongPlayer::ballLocationInput(Vector<PongBall>& balls)
{
	const PongBall* ball = getClosestBall(*this, balls);
	if (ball) position.y = clamp(ball->position.y, -0.9f, 0.9f);
	shoot = true;
}

void PongPlayer::mouseInput(const Vec3 cursorWorldPosition)
{
	position.y = cursorWorldPosition.y;
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void PongPlayer::aiInput(Vector<PongBall>& balls, const Float deltaTime)
{
	shoot = randomIntFast(1000) < 1;
	const PongBall* ball = getClosestBall(*this, balls);
	if (ball)
	{
		const Float reactionTime = deltaTime * difficulty;
		const Float predictedPosition = ball->position.y + (position.x - ball->position.x) / ball->velocity.x * ball->velocity.y;
		target = (target + reactionTime * ((predictedPosition + ball->position.y) / 2)) / (1 + reactionTime);
		const Float targetVelocity = target - position.y;
		velocity += targetVelocity - velocity;
		position.y += velocity;
		position.y = clamp(position.y, -0.9f, 0.9f);
	}
}

void PongPlayer::keyboardInput(Window& window, Int up, Int down, Int shoot) {
	if (window.isKeyPressed(up)) position.y += window.renderer.deltaTime() * 2;
	if (window.isKeyPressed(down)) position.y -= window.renderer.deltaTime() * 2;
	this->shoot = window.isKeyPressed(shoot);
}

PongBall* getClosestBall(const PongPlayer& player, Vector<PongBall>& balls)
{
	PongBall* closestBall = nullptr;

	Float closestX = 100;
	for (PongBall& ball : balls)
	{
		const Float delta = ball.position.x - player.position.x;
		if ((delta >= 0 && ball.velocity.x <= 0) || (delta <= 0 && ball.velocity.x >= 0))
		{
			const Float distance = abs(delta);
			if (distance < closestX)
			{
				closestX = distance;
				closestBall = &ball;
			}
		}
	}

	return closestBall;
}
