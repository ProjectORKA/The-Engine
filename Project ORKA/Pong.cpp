
#include "Pong.hpp"
#include "Window.hpp"
#include "Random.hpp"

Float ballSpeedReduction = 1.5;
Float paddleSpeed = 2.5;

void Ball::render(Renderer& renderer) {
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01, 0.01, 0.5));
	renderer.renderMesh("centeredCube");
}
void Ball::update(Float deltaTime, PongPlayer players[2]) {
	if (stuckToPaddle) {
		if (stuckToPaddle1) {
			position.x = -0.85;
			position.y = players[0].position.y;
			if (players[0].shoot) {
				stuckToPaddle = false;
				velocity.x = 1;
				velocity.y = randomFloat(-1, 1);
				speed = 2;
			}
		}
		else {
			position.x = 0.85;
			position.y = players[1].position.y;
			if (players[1].shoot) {
				stuckToPaddle = false;
				velocity.x = -1;
				velocity.y = randomFloat(-1, 1);
				speed = 2;
			}
		}
	}
	else {
		//ball is not stuck to paddle

		speed += deltaTime / 45.0f;

		velocity = normalize(velocity) * speed;
		position += velocity * deltaTime;
		if (velocity.x > 0) {
			//ball is moving to the right
			if (position.x > 1) {
				//ball is in goal
				players[0].score++;
				beep();
				//player score increased
				stuckToPaddle = true;
				stuckToPaddle1 = true;
				//ball will be attached to paddle 1
				players[1].difficulty *= PONG_DIFFICULTY_MODIFIER;
			}
			else {
				if (position.x > 0.895 && position.x < 0.95 && abs(position.y - players[1].position.y) < 0.1) {
					//ball did collide
					velocity.x = -1;
					velocity.y = randomFloat(-1, 1);
				}
			}
		}
		else {
			//ball is moving to the left
			if (position.x < -1) {
				//ball is in goal
				players[1].score++;
				beep();
				//player score increased
				stuckToPaddle = true;
				stuckToPaddle1 = false;
				//ball will be attached to paddle 2
				players[1].difficulty /= PONG_DIFFICULTY_MODIFIER;
			}
			else {
				if (position.x < -0.895 && position.x > -0.95 && abs(position.y - players[0].position.y) < 0.1) {
					//ball did collide
					velocity.x = 1;
					velocity.y = randomFloat(-1, 1);
				}
			}
		}
	}

	if (position.y > +1 & velocity.y > 0) {
		velocity.y = -velocity.y;
	}
	if (position.y < -1 & velocity.y < 0) {
		velocity.y = -velocity.y;
	}
}

void PongPlayer::keyboardInput(Float deltaTime) {
	if (moveUp) position.y += paddleSpeed * deltaTime;
	if (moveDown) position.y -= paddleSpeed * deltaTime;

	position.y = clamp(position.y, -0.9f, 0.9f);
}
void PongPlayer::mouseInput(Vec3 cursorWorldPosition) {
	position.y = cursorWorldPosition.y;

	position.y = clamp(position.y, -0.9f, 0.9f);
}
void PongPlayer::ballLocationInput(Vector<Ball>& balls) {
	Ball* ball = getClosestBall(*this, balls);
	if (ball) {
		position.y = ball->position.y;

		position.y = clamp(position.y, -0.9f, 0.9f);
		shoot = true;
	}
}
void PongPlayer::aiInput(Vector<Ball>& balls, Float deltaTime) {

	shoot = random(1000) < 1;

	Ball* ball = getClosestBall(*this, balls);
	if (ball) {
		//if ((ball.velocity.x > 0 && ball.position.x > randomFloat(-4, 0)) || (ball.velocity.x < 0 && ball.position.x < randomFloat(-4, 0))) {
		Float reactionTime = deltaTime * difficulty;

		Float predictedPosition = ball->position.y + ((position.x - ball->position.x) / ball->velocity.x) * ball->velocity.y;

		target = (target + reactionTime * ((predictedPosition + ball->position.y) / 2)) / (1 + reactionTime);

		Float targetVelocity = target - position.y;

		velocity += (targetVelocity - velocity);

		position.y += velocity;
		//}

		position.y = clamp(position.y, -0.9f, 0.9f);
	}
}

void Pong::render(TiledRectangle area, Renderer& renderer) {

	players[0].position.x = -0.9;
	players[1].position.x = +0.9;

	if (balls.size() <= 10)balls.emplace_back();

	renderer.apectCorrectNormalizedSpace();
	Vec2 normalizedCursorPosition = Vec2(2) * ((Vec2(inputManager.cursorPosition) / Vec2(renderer.framebufferSystem.framebufferSize)) - Vec2(0.5));
	Vec3 cursorWorldPos = inverse(renderer.uniforms().pMatrix()) * Vec4(normalizedCursorPosition, 0, 1);


	//players[1].ballLocationInput(ball);

	for (Ball& ball : balls) ball.update(renderer.time.delta, players);


	//players[0].keyboardInput(renderer.renderTime.delta);
	players[0].mouseInput(cursorWorldPos);
	//players[0].aiInput(balls, renderer.renderTime.delta);
	//players[0].ballLocationInput(balls);

	//players[1].ballLocationInput(balls);
	players[1].aiInput(balls, renderer.time.delta);


	///////////////////////////////////////////////////////////////////////

	renderer.clearColor(Color(0, 0, 0, 1));

	renderer.apectCorrectNormalizedSpace();
	renderer.useShader("color");
	renderer.uniforms().customColor() = Vec4(1);

	//paddle 1
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.01, 0.2, 0.5));
	renderer.renderMesh("centeredCube");
	//paddle 2
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.01, 0.2, 0.5));
	renderer.renderMesh("centeredCube");
	//ball
	for (Ball& ball : balls)ball.render(renderer);

	//walls
	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5));
	renderer.renderMesh("centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5));
	renderer.renderMesh("centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5));
	renderer.renderMesh("centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5));
	renderer.renderMesh("centeredCube");

	Float height = renderer.framebufferSystem.framebufferSize.y;
	Float width = renderer.framebufferSystem.framebufferSize.x;

	//text
	FontStyle style;
	style.absoluteSize = 50;

	renderer.screenSpace();
	renderer.renderText(toString(players[0].score), Vec2(10, height - 100), style);
	renderer.renderText(toString(players[1].score), Vec2(width - 100, height - 100), style);
	renderer.renderText(toString(players[1].difficulty), Vec2(10, height - 200), style);
	renderer.renderText(toString(balls[0].speed), Vec2(10, height - 300), style);
	renderer.renderText(toString(balls[0].speed), Vec2(10, height - 300), style);
	renderer.renderText(toString(1.0f / renderer.time.delta), Vec2(50, 50), fonts.paragraph);
}
void Pong::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {

	Bool pressed = action != ActionState::Release;

	switch (key) {
	case Key::W:players[0].moveUp = pressed;
		break;
	case Key::S:players[0].moveDown = pressed;
		break;
	case Key::SPACE:players[0].shoot = pressed;
		break;
	case Key::UP:players[1].moveUp = pressed;
		break;
	case Key::DOWN:players[1].moveDown = pressed;
		break;
	case Key::LEFT:players[1].shoot = pressed;
		break;
	case Key::P:balls.emplace_back();
		break;
	}
}
void Pong::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	players[0].shoot = (action == ActionState::Press || action == ActionState::Repeat);
}

Ball* getClosestBall(PongPlayer& player, Vector<Ball>& balls) {
	Ball* closestBall = nullptr;

	Float closestX = 100;
	for (Ball& ball : balls) {
		Float delta = ball.position.x - player.position.x;
		if ((delta >= 0 && ball.velocity.x <= 0) || (delta <= 0 && ball.velocity.x >= 0)) {
			Float distance = abs(delta);
			if (distance < closestX) {
				closestX = distance;
				closestBall = &ball;
			}
		}
	}

	return closestBall;
}