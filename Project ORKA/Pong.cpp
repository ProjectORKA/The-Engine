#include "Pong.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "AudioEngine.hpp"
#include "ResourceManager.hpp"
#include "Debug.hpp"

Float ballSpeedReduction = 1.5;
Float paddleSpeed        = 2.5;

namespace
{
	constexpr const Char* pongScoreSoundId = "pong_score";
	constexpr const Char* pongScoreWav      = "score.wav";

	constexpr const Char* pongBounceSoundId = "pong_bounce";
	constexpr const Char* pongBounceWav     = "bounce.wav";
	
	constexpr const Char* pongShootSoundId  = "pong_shoot";
	constexpr const Char* pongShootWav      = "shoot.wav";

	inline void playPongScoreSound()
	{
		if (audioEngine.has(pongScoreSoundId)) audioEngine.play(pongScoreSoundId);
		else beep();
	}

	inline void playPongBounceSound()
	{
		if (audioEngine.has(pongBounceSoundId)) audioEngine.play(pongBounceSoundId);
		else beep();
	}

	inline void playPongShootSound()
	{
		if (audioEngine.has(pongShootSoundId)) audioEngine.play(pongShootSoundId);
		else beep();
	}

	inline void loadPongSounds()
	{
		const Path dataSounds = resourceManager.getOrkaDataPath() / "Sounds";

		const Path wavPath = dataSounds / pongScoreWav;
		audioEngine.load(pongScoreSoundId, wavPath, false);

		const Path bouncePath = dataSounds / pongBounceWav;
		audioEngine.load(pongBounceSoundId, bouncePath, false);

		const Path shootPath = dataSounds / pongShootWav;
		audioEngine.load(pongShootSoundId, shootPath, false);
	}
}

void PongRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void PongPlayer::update(Window& window)
{
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void PongRenderer::update(Window& window)
{
	const Float delta = window.renderer.deltaTime();
	if(window.pressed(moveUpButton1)) players[0].position.y += paddleSpeed * delta;
	if(window.pressed(moveDownButton1)) players[0].position.y -= paddleSpeed * delta;
	players[0].shoot = window.pressed(shootButton1) || window.pressed(shootButton1secondary);

	if(window.pressed(moveUpButton2)) players[1].position.y += paddleSpeed * delta;
	if(window.pressed(moveDownButton2)) players[1].position.y -= paddleSpeed * delta;
	// players[1].shoot = w.pressed(shootButton2);

	for(PongPlayer& p : players) p.update(window);
}

void PongRenderer::connect(GameSimulation& simulation) {}

void PongPlayer::ballPositionInput(Vector<Ball>& balls)
{
	const Ball* ball = getClosestBall(*this, balls);
	if(ball)
	{
		position.y = ball->position.y;
		position.y = clamp(position.y, -0.9f, 0.9f);
	}
}

void PongRenderer::inputEvent(Window& window, InputEvent& input) {}

void PongPlayer::mouseInput(const DVec2 cursorWorldPosition)
{
	position.y = cursorWorldPosition.y * 2 - 1;
	position.y = clamp(position.y, -0.9f, 0.9f);
}

void Ball::update(const Float deltaTime, PongPlayer players[2], PongParticleSystem* collisionParticles)
{
	auto spawnImpactParticles = [&](const Vec2& impactPosition, const Vec2& incomingVelocity)
	{
		if(!collisionParticles) return;

		constexpr Int particlesPerImpact = 18;
		for(Int i = 0; i < particlesPerImpact; i++) collisionParticles->spawn(impactPosition, incomingVelocity);
	};

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
				velocity.y    = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed  = 2;
				playPongShootSound();
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
				velocity.y    = randomFloatFast(-1.0f, 1.0f);
				desiredSpeed  = 2;
				playPongShootSound();
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
				playPongScoreSound();
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
					spawnImpactParticles(position, velocity);
					velocity.x = -1;
					velocity.y = randomFloatFast(-1.0f, 1.0f);
					playPongBounceSound();
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
				playPongScoreSound();
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
					spawnImpactParticles(position, velocity);
					velocity.x = 1;
					velocity.y = randomFloatFast(-1.0f, 1.0f);
					playPongBounceSound();
				}
			}
		}
	}

	if(position.y > +1 && velocity.y > 0)
	{
		spawnImpactParticles(position, velocity);
		velocity.y = -velocity.y;
		playPongBounceSound();
	}
	if(position.y < -1 && velocity.y < 0)
	{
		spawnImpactParticles(position, velocity);
		velocity.y = -velocity.y;
		playPongBounceSound();
	}
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
	shoot = randomIntFast(1000) < 1;
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

void PongRenderer::create(Window& window)
{
	while(balls.size() < ballCount) balls.emplace_back();

	loadPongSounds();

	GameRenderer::create(window);
}

void Ball::render(Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(position, 0)), Vec3(0.01, 0.01, 0.5)));
	renderer.renderMesh("centeredCube");
}

void PongRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;
	r.setCulling(true);
	r.setDepthTest(true);

	// Clear framebuffer
	const Color backgroundColor = Color(0.05, 0.05, 0.05, 1.0);
	framebuffer.clearColor(r, backgroundColor);
	framebuffer.clearDepth(r);
	
	// Bind framebuffer for rendering
	r.disableScissor();

	players[0].position.x = -0.9f;
	players[1].position.x = +0.9f;

	r.aspectCorrectNormalizedSpace();
	
	const DVec2 cursorWorldPos = getMousePosBottomLeft(window) / DVec2(getSize());

	for(Ball& ball : balls) ball.update(r.time.getDelta(), players, &collisionParticles);

	players[0].mouseInput(cursorWorldPos);
	players[1].aiInput(balls, r.time.getDelta());
	collisionParticles.update(r.time.getDelta());

	// Clear and render game
	r.clearBackground(backgroundColor);
	r.aspectCorrectNormalizedSpace();
	r.useShader("color");
	r.uniforms().setCustomColor(Vec4(1));

	// paddle 1
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh("centeredCube");
	// paddle 2
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh("centeredCube");
	// ball
	for(Ball& ball : balls) ball.render(r);
	r.setAlphaBlending(true);
	collisionParticles.render(r);
	r.setAlphaBlending(false);

	// walls
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh("centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh("centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh("centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh("centeredCube");

	// Render UI elements
	const IVec2 renderSize = getSize();
	const Float width      = static_cast<Float>(renderSize.x);
	const Float height     = static_cast<Float>(renderSize.y);

	// text
	r.setDepthTest(false);
	r.setCulling(false);
	r.uniforms().setMMatrix(Matrix(1));
	r.screenSpaceFromBottomLeft();
	r.fill(Color(1));
	r.fontSize(50.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::top);

	r.text(toString(players[0].score), Vec2(10.0f, height - 100.0f));
	r.text(toString(players[1].difficulty), Vec2(10.0f, height - 200.0f));
	r.textRenderSystem.alignText(Alignment::right, Alignment::top);
	r.text(toString(players[1].score), Vec2(width - 100.0f, height - 100.0f));
}

void PongRenderer::renderInteractive(Window& window) {}
