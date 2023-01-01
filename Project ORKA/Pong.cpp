
#include "Pong.hpp"
#include "Window.hpp"
#include "Random.hpp"

Float ballSpeedReduction = 1.5;
Float paddleSpeed = 2.5;

void Ball::render(Engine& engine, Renderer& renderer) {
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01, 0.01, 0.5));
	renderer.renderMesh(engine, "centeredCube");
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
				desiredSpeed = 2;
			}
		}
		else {
			position.x = 0.85;
			position.y = players[1].position.y;
			if (players[1].shoot) {
				stuckToPaddle = false;
				velocity.x = -1;
				velocity.y = randomFloat(-1, 1);
				desiredSpeed = 2;
			}
		}
	}
	else {
		//ball is not stuck to paddle

		desiredSpeed += deltaTime / 45.0f;

		velocity = normalize(velocity) * desiredSpeed;
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

void PongPlayer::mouseInput(Vec3 cursorWorldPosition) {
	position.y = cursorWorldPosition.y;

	position.y = clamp(position.y, -0.9f, 0.9f);
}
void PongPlayer::ballLocationInput(Vector<Ball>& balls) {
	Ball* ball = getClosestBall(*this, balls);
	if (ball) {
		position.y = ball->position.y;

		position.y = clamp(position.y, -0.9f, 0.9f);
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

void PongPlayer::update(Window& window) {
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void Pong::create(Window& window) {
	//window.input.add("player1shoot", ButtonType::Key, Key::SPACE);
	//window.input.add("player1up", ButtonType::Key, Key::W);
	//window.input.add("player1down", ButtonType::Key, Key::S);
	//window.input.add("player2shoot", ButtonType::Key, Key::LEFT);
	//window.input.add("player2up", ButtonType::Key, Key::UP);
	//window.input.add("player2down", ButtonType::Key, Key::DOWN);
	//window.input.add("spawnBall", ButtonType::Key, Key::P);
}
void Pong::render(Engine& engine, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	////input
	//players[0].shoot = window.input.holding("player1shoot");
	//players[0].moveUp = window.input.holding("player1up");
	//players[0].moveDown = window.input.holding("player1down");

	//players[1].shoot = window.input.holding("player2shoot");
	//players[1].moveUp = window.input.holding("player2up");
	//players[1].moveDown = window.input.holding("player2down");

	//if(window.input.holding("spawnBall")) balls.emplace_back();


	players[0].position.x = -0.9;
	players[1].position.x = +0.9;

	if (balls.size() <= 10)balls.emplace_back();

	renderer.aspectCorrectNormalizedSpace();
	Vec2 normalizedCursorPosition = Vec2(2) * ((Vec2(window.mousePosBotLeft) / Vec2(renderer.framebufferSystem.windowSize)) - Vec2(0.5));
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

	renderer.aspectCorrectNormalizedSpace();
	renderer.useShader(engine,"color");
	renderer.uniforms().customColor() = Vec4(1);

	//paddle 1
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.01, 0.2, 0.5));
	renderer.renderMesh(engine, "centeredCube");
	//paddle 2
	renderer.uniforms().mMatrix() =
		scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.01, 0.2, 0.5));
	renderer.renderMesh(engine, "centeredCube");
	//ball
	for (Ball& ball : balls)ball.render(engine, renderer);

	//walls
	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5));
	renderer.renderMesh(engine, "centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5));
	renderer.renderMesh(engine, "centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5));
	renderer.renderMesh(engine, "centeredCube");

	renderer.uniforms().mMatrix() = scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5));
	renderer.renderMesh(engine, "centeredCube");

	Float height = renderer.framebufferSystem.windowSize.y;
	Float width = renderer.framebufferSystem.windowSize.x;

	//text
	FontStyle style;
	style.absoluteSize = 50;

	renderer.screenSpace();
	renderer.renderText(engine, toString(players[0].score), Vec2(10, height - 100), style);
	renderer.renderText(engine, toString(players[1].score), Vec2(width - 100, height - 100), style);
	renderer.renderText(engine, toString(players[1].difficulty), Vec2(10, height - 200), style);
	renderer.renderText(engine, toString(balls[0].desiredSpeed), Vec2(10, height - 300), style);
	renderer.renderText(engine, toString(balls[0].desiredSpeed), Vec2(10, height - 300), style);
	renderer.renderText(engine, toString(1.0f / renderer.time.delta), Vec2(50, 50), fonts.paragraph);
}

void Pong::update(Window& window) {
	Float delta = window.renderer.deltaTime();
	if (window.pressed(moveUpButton1)) players[0].position.y += paddleSpeed * delta;
	if (window.pressed(moveDownButton1)) players[0].position.y -= paddleSpeed * delta;
	players[0].shoot = window.pressed(shootButton1) || window.pressed(shootButton1secondary);

	if (window.pressed(moveUpButton2)) players[1].position.y += paddleSpeed * delta;
	if (window.pressed(moveDownButton2)) players[1].position.y -= paddleSpeed * delta;
	players[1].shoot = window.pressed(shootButton2);

	for (PongPlayer& p : players) {
		p.update(window);
	}
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