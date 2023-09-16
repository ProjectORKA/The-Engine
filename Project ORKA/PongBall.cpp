
#include "PongBall.hpp"
#include "PongPlayer.hpp"
#include "Random.hpp"

void PongBall::update(const Float deltaTime, PongPlayer players[2])
{
	trail.push_back(position);
	if (trail.size() > 10) trail.pop_front();

	if (stuckToPaddle)
	{
		if (stuckToPaddle1)
		{
			position.x = -0.85f;
			position.y = players[0].position.y;
			if (players[0].shoot)
			{
				stuckToPaddle = false;
				velocity.x = 1;
				velocity.y = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed = 2;
			}
		}
		else
		{
			position.x = 0.85f;
			position.y = players[1].position.y;
			if (players[1].shoot)
			{
				stuckToPaddle = false;
				velocity.x = -1;
				velocity.y = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed = 2;
			}
		}
	}
	else
	{
		// ball is not stuck to paddle

		desiredSpeed += deltaTime / 45.0f;

		velocity = normalize(velocity) * desiredSpeed;
		position += velocity * deltaTime;
		if (velocity.x > 0)
		{
			// ball is moving to the right
			if (position.x > 1)
			{
				// ball is in goal
				players[0].score++;
				beep();
				// player score increased
				stuckToPaddle = true;
				stuckToPaddle1 = true;
				// ball will be attached to paddle 1
				players[1].difficulty *= PONG_DIFFICULTY_MODIFIER;
			}
			else
			{
				if (position.x > 0.895 && position.x < 0.95 && abs(position.y - players[1].position.y) < 0.1)
				{
					// ball did collide
					velocity.x = -1;
					velocity.y = randomFloatFast(-1.0f, 1.0f);
				}
			}
		}
		else
		{
			// ball is moving to the left
			if (position.x < -1)
			{
				// ball is in goal
				players[1].score++;
				beep();
				// player score increased
				stuckToPaddle = true;
				stuckToPaddle1 = false;
				// ball will be attached to paddle 2
				players[1].difficulty /= PONG_DIFFICULTY_MODIFIER;
			}
			else
			{
				if (position.x < -0.895 && position.x > -0.95 && abs(position.y - players[0].position.y) < 0.1)
				{
					// ball did collide
					velocity.x = 1;
					velocity.y = randomFloatFast(-1.0f, 1.0f);
				}
			}
		}
	}

	if (position.y > +1 & velocity.y > 0) velocity.y = -velocity.y;
	if (position.y < -1 & velocity.y < 0) velocity.y = -velocity.y;
}

void PongBall::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01, 0.01, 0.5)));
	renderer.renderMesh(resourceManager, "centeredCube");
	renderer.line(trail);
}