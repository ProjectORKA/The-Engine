#include "Pong.hpp"
#include "Window.hpp"
#include "Random.hpp"

Float ballSpeedReduction = 1.5;
Float paddleSpeed        = 2.5;

void Pong::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Pong", Area(1920, 1080), true, true, WindowState::Fullscreen, pongRenderer, resourceManager);
	ui.run();
}

void PongRenderer::destroy(Window& window) {}

void PongRenderer::update(Window& w)
{
	const Float delta = w.renderer.deltaTime();
	if(w.pressed(moveUpButton1)) players[0].position.y += paddleSpeed * delta;
	if(w.pressed(moveDownButton1)) players[0].position.y -= paddleSpeed * delta;
	players[0].shoot = w.pressed(shootButton1) || w.pressed(shootButton1secondary);

	if(w.pressed(moveUpButton2)) players[1].position.y += paddleSpeed * delta;
	if(w.pressed(moveDownButton2)) players[1].position.y -= paddleSpeed * delta;
	// players[1].shoot = w.pressed(shootButton2);

	for(PongPlayer& p : players) p.update(w);
}

void PongPlayer::update(Window& window)
{
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void PongPlayer::ballLocationInput(Vector<Ball>& balls)
{
	const Ball* ball = getClosestBall(*this, balls);
	if(ball)
	{
		position.y = ball->position.y;

		position.y = clamp(position.y, -0.9f, 0.9f);
	}
}

void PongRenderer::inputEvent(Window& window, InputEvent input) {}

void PongPlayer::mouseInput(const Vec3 cursorWorldPosition)
{
	position.y = cursorWorldPosition.y;

	position.y = clamp(position.y, -0.9f, 0.9f);
}

void Ball::update(const Float deltaTime, PongPlayer players[2])
{
	if(stuckToPaddle)
	{
		if(stuckToPaddle1)
		{
			position.x = -0.85f;
			position.y = players[0].position.y;
			if(players[0].shoot)
			{
				stuckToPaddle = false;
				velocity.x    = 1;
				velocity.y    = randomFloat(-1, 1);
				desiredSpeed  = 2;
			}
		}
		else
		{
			position.x = 0.85f;
			position.y = players[1].position.y;
			if(players[1].shoot)
			{
				stuckToPaddle = false;
				velocity.x    = -1;
				velocity.y    = randomFloat(-1, 1);
				desiredSpeed  = 2;
			}
		}
	}
	else
	{
		// ball is not stuck to paddle

		desiredSpeed += deltaTime / 45.0f;

		velocity = normalize(velocity) * desiredSpeed;
		position += velocity * deltaTime;
		if(velocity.x > 0)
		{
			// ball is moving to the right
			if(position.x > 1)
			{
				// ball is in goal
				players[0].score++;
				beep();
				// player score increased
				stuckToPaddle  = true;
				stuckToPaddle1 = true;
				// ball will be attached to paddle 1
				players[1].difficulty *= PONG_DIFFICULTY_MODIFIER;
			}
			else
			{
				if(position.x > 0.895 && position.x < 0.95 && abs(position.y - players[1].position.y) < 0.1)
				{
					// ball did collide
					velocity.x = -1;
					velocity.y = randomFloat(-1, 1);
				}
			}
		}
		else
		{
			// ball is moving to the left
			if(position.x < -1)
			{
				// ball is in goal
				players[1].score++;
				beep();
				// player score increased
				stuckToPaddle  = true;
				stuckToPaddle1 = false;
				// ball will be attached to paddle 2
				players[1].difficulty /= PONG_DIFFICULTY_MODIFIER;
			}
			else
			{
				if(position.x < -0.895 && position.x > -0.95 && abs(position.y - players[0].position.y) < 0.1)
				{
					// ball did collide
					velocity.x = 1;
					velocity.y = randomFloat(-1, 1);
				}
			}
		}
	}

	if(position.y > +1 & velocity.y > 0) velocity.y = -velocity.y;
	if(position.y < -1 & velocity.y < 0) velocity.y = -velocity.y;
}

Ball* getClosestBall(const PongPlayer& player, Vector<Ball>& balls)
{
	Ball* closestBall = nullptr;

	Float closestX = 100;
	for(Ball& ball : balls)
	{
		const Float delta = ball.position.x - player.position.x;
		if((delta >= 0 && ball.velocity.x <= 0) || (delta <= 0 && ball.velocity.x >= 0))
		{
			const Float distance = abs(delta);
			if(distance < closestX)
			{
				closestX    = distance;
				closestBall = &ball;
			}
		}
	}

	return closestBall;
}

void PongPlayer::aiInput(Vector<Ball>& balls, const Float deltaTime)
{
	shoot = random(1000) < 1;
	logDebug(shoot);
	const Ball* ball = getClosestBall(*this, balls);
	if(ball)
	{
		// if ((ball.velocity.x > 0 && ball.position.x > randomFloat(-4, 0)) || (ball.velocity.x < 0 && ball.position.x < randomFloat(-4, 0))) {
		const Float reactionTime = deltaTime * difficulty;

		const Float predictedPosition = ball->position.y + (position.x - ball->position.x) / ball->velocity.x * ball->velocity.y;

		target = (target + reactionTime * ((predictedPosition + ball->position.y) / 2)) / (1 + reactionTime);

		const Float targetVelocity = target - position.y;

		velocity += targetVelocity - velocity;

		position.y += velocity;
		// }

		position.y = clamp(position.y, -0.9f, 0.9f);
	}
}

void PongRenderer::create(ResourceManager& resourceManager, Window& window) {}

void Ball::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01, 0.01, 0.5)));
	renderer.renderMesh(resourceManager, "centeredCube");
}

void PongRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	// //input
	// players[0].shoot = window.input.holding("player1shoot");
	// players[0].moveUp = window.input.holding("player1up");
	// players[0].moveDown = window.input.holding("player1down");

	// players[1].shoot = window.input.holding("player2shoot");
	// players[1].moveUp = window.input.holding("player2up");
	// players[1].moveDown = window.input.holding("player2down");

	// if(window.input.holding("spawnBall")) balls.emplace_back();

	players[0].position.x = -0.9f;
	players[1].position.x = +0.9f;

	if(balls.size() <= 1) balls.emplace_back();

	r.aspectCorrectNormalizedSpace();
	const Vec2 normalizedCursorPosition = Vec2(2) * (Vec2(window.mousePosBotLeft) / Vec2(r.getWindowSize()) - Vec2(0.5));
	const Vec3 cursorWorldPos           = inverse(r.uniforms().getPMatrix()) * Vec4(normalizedCursorPosition, 0, 1);

	// players[1].ballLocationInput(ball);

	for(Ball& ball : balls) ball.update(r.time.delta, players);

	// players[0].keyboardInput(renderer.renderTime.delta);
	players[0].mouseInput(cursorWorldPos);
	// players[0].aiInput(balls, renderer.renderTime.delta);
	// players[0].ballLocationInput(balls);

	// players[1].ballLocationInput(balls);
	players[1].aiInput(balls, r.time.delta);

	///////////////////////////////////////////////////////////////////////

	r.clearBackground(Color(0, 0, 0, 1));

	r.aspectCorrectNormalizedSpace();
	r.useShader(resourceManager, "color");
	r.uniforms().setCustomColor(Vec4(1));

	// paddle 1
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");
	// paddle 2
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");
	// ball
	for(Ball& ball : balls) ball.render(resourceManager, r);

	// walls
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	const Float height = r.getWindowHeight();
	const Float width  = r.getWindowWidth();

	// text
	FontStyle style;
	style.absoluteSize = 50;
	r.uniforms().setMMatrix(Matrix(1));
	r.screenSpace();
	r.fill(Color(1));
	r.textRenderSystem.alignText(Alignment::left, Alignment::top);
	r.textRenderSystem.setStyle(style);
	r.textRenderSystem.render(resourceManager, r, toString(players[0].score), Vec2(10, height - 100));
	r.textRenderSystem.render(resourceManager, r, toString(players[1].difficulty), Vec2(10, height - 200));
	r.textRenderSystem.alignText(Alignment::right, Alignment::top);
	r.textRenderSystem.render(resourceManager, r, toString(players[1].score), Vec2(width - 100, height - 100));
	r.textRenderSystem.setStyle(fonts.debug);
}

void PongRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
