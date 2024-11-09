#include "TripleNine.hpp"
#include "JobSystem.hpp"
#include "Window.hpp"

#ifdef TRIPLE_NINE_ENEMY
void TripleNineEnemy::update(Window& window) {}

void TripleNineEnemy::destroy(Window& window) {}

const Vector<TripleNineEnemy>& TripleNineGameState::getEnemies()
{
	return enemies;
}

void TripleNineSimulation::createEnemy()
{
	TripleNineEnemy& newEnemy =gameState.createEnemy();
}

TripleNineEnemy& TripleNineGameState::createEnemy()
{
	enemies.emplace_back();
	enemies.back().id = static_cast<Index>(enemies.size()) - static_cast<Index>(1);
	return enemies.back();
}

void TripleNineEnemy::inputEvent(Window& window, InputEvent input) {}

void TripleNineEnemy::create(Window& window) {}

void TripleNineEnemy::setPosition(const Vec3 position) {
	this->position = position;
}
#endif

// GameState

void TripleNineGameState::updatePlayer(const Token playerToken, const Vec3 position)
{
	for (TripleNinePlayerProxy& player : players)
	{
		if (player.playerToken == playerToken)
		{
			player.position = position;
			return;
		}
	}
	players.push_back({playerToken, position});
}

Vec3 TripleNineGameState::getPlayerPosition(const Token playerToken) const
{
	for (auto& player : players)
	{
		if (player.playerToken == playerToken)
		{
			return player.position;
		}
	}
	return Vec3(0);
}

ULL TripleNineGameState::getTick() const
{
	return gameTick;
}

// Simulation

void TripleNineSimulation::create()
{
	server.create();

	#ifdef TRIPLE_NINE_ENEMY
	for (Int i = 0; i < 100; i++) createEnemy();
	#endif
}

void TripleNineSimulation::destroy()
{
	server.destroy();
}

void TripleNineSimulation::update(Float delta)
{
	server.processEvents();
}

void TripleNineSimulation::updatePlayer(const Byte playerId, const Vec3 position)
{
	gameState.updatePlayer(playerId, position);
}

// Player

void TripleNinePlayer::jump()
{
	if (isMoving)
	{
		velocity.z = 0;
		velocity += Vec3(0, 0, minJumpVelocity);
	}
	else
	{
		velocity.z = 0;
		velocity += Vec3(0, 0, lerp(minJumpVelocity, maxJumpVelocity, jumpCharge));
		jumpCharge = 0;
	}
	// queueJump = false;
	onGround = false;
	state    = State::Jumping;
}

void TripleNinePlayer::collisionResponse()
{
	if (velocity.z < 0) velocity.z = 0;
	if (position.z < 0) position.z = 0;
}

void TripleNinePlayer::update(Window& window)
{
	// set up temporary data
	const Renderer& renderer = window.renderer;
	const Float     delta    = renderer.deltaTime();
	// calculate player vectors
	forwardVector = normalize(Vec3(sin(camera.getRotationZ()), cos(camera.getRotationZ()), 0));
	rightVector   = Vec3(forwardVector.y, -forwardVector.x, 0);
	// process input
	if (window.pressed(aim))
	{
		approach(fov, fovScoped, delta * scopeSpeed);
		approach(mouseSensitivity, mouseSensitivityScoped, delta * scopeSpeed);
	}
	else
	{
		approach(fov, fovDefault, delta * scopeSpeed);
		approach(mouseSensitivity, mouseSensitivityDefault, delta * scopeSpeed);
	}
	camera.setFieldOfView(fov);
	if (window.capturing) targetCameraRotation += window.mouseDelta * DVec2(mouseSensitivity);
	movementControl = Vec3(0);
	if (window.pressed(forward)) movementControl += forwardVector;
	if (window.pressed(backward)) movementControl -= forwardVector;
	if (window.pressed(right)) movementControl += rightVector;
	if (window.pressed(left)) movementControl -= rightVector;
	upperBodyLeanControl = 0;
	if (window.pressed(leanRight) && onGround) upperBodyLeanControl += 1;
	if (window.pressed(leanLeft) && onGround) upperBodyLeanControl -= 1;
	movementInput = movementControl != Vec3(0);
	if (onGround)
	{
		if (window.pressed(crouch))
		{
			if (window.pressed(holdJump))
			{
				if (movementInput)
				{
					state = State::Crawling;
				}
				else
				{
					state = State::Proning;
				}
			}
			else
			{
				if (movementInput)
				{
					state = State::Sneaking;
				}
				else
				{
					state = State::Crouching;
				}
			}
		}
		else
		{
			if (movementInput)
			{
				if (window.pressed(run))
				{
					state = State::Sprinting;
				}
				else
				{
					state = State::Walking;
				}
			}
			else
			{
				state = State::Standing;
			}
		}
	}
	else
	{
		if (velocity.z >= 0)
		{
			state = State::Jumping;
		}
		else
		{
			state = State::Falling;
		}
	}
	// update values
	// running
	if (state == State::Sprinting)
	{
		speedControl += delta * speedControlAcceleration;
	}
	else
	{
		speedControl -= delta * speedControlDeceleration;
	}
	speedControl = clamp(speedControl, 0.0f, 1.0f);
	camera.rotate(targetCameraRotation);
	calculatePhysics(window);
	if (isCollidingWithGround())
	{
		onGround         = true;
		doubleJumpCharge = true;
		if (window.pressed(holdJump)) jump();
		// if (queueJump) {
		//	queueJump = false;
		// }
		collisionResponse();
		if (movementInput)
		{
			actualFriction = movementFriction;
		}
		else
		{
			actualFriction = stopFriction;
		}
		actualFriction *= normalFrictionFactor;
		velocity /= 1 + delta * actualFriction;
	}
	else
	{
		onGround = false;
	}
	if (state == State::Jumping) debugCurrentMaxJumpHeight = position.z;
	isMoving = movementControl != Vec3(0);
	// air strafing
	if (movementInput && !onGround)
	{
		const Vec3  strafeVector     = movementControl * Vec3(maxStrafeSpeed);
		const Vec3  projectionVector = proj(velocity, movementControl);
		const Vec3  deltaVec         = strafeVector - projectionVector;
		const Float velocityAddMax   = length(deltaVec);
		if (length(projectionVector) < velocityAddMax) velocity += movementControl * min(delta * maxStrafeAcceleration, velocityAddMax);
	}
	// movement speed
	targetSpeed = lerp(walkingSpeed, runningSpeed, speedControl);
	if (movementInput && onGround)
	{
		// add speed to direction
		approach(actualInputSpeed, targetSpeed, delta * 2);
		movementControl = normalize(movementControl);
		movementControl *= actualInputSpeed;
		approach(velocity.x, movementControl.x, 10 * delta);
		approach(velocity.y, movementControl.y, 10 * delta);
		movementControl = {0, 0, 0};
	}
	actualSpeed = length(velocity);
	// head position
	calculateHeadPosition(window, delta);
	// reset delta
	targetCameraRotation = DVec2(0);
}

Bool TripleNinePlayer::isCollidingWithGround() const
{
	return position.z <= 0;
}

void TripleNinePlayer::calculatePhysics(const Window& window)
{
	// step 1
	const Vec3 velocity1 = velocity + acceleration * window.renderer.deltaTime() / Vec3(2);
	// step 2
	position += velocity1 * window.renderer.deltaTime();
	// step 3 calculate forces
	// player keeps jumping higher when holding space but falls faster when releasing it
	if (!onGround && window.pressed(holdJump))
	{
		acceleration = Vec3(0, 0, -airTimeGravity);
	}
	else
	{
		acceleration = Vec3(0, 0, -gravity);
	}
	// step4
	const Vec3 velocity2 = velocity1 + acceleration * window.renderer.deltaTime() / Vec3(2);
	velocity             = velocity2 * pow(airResistance, window.renderer.deltaTime());
}

void TripleNinePlayer::inputEvent(Window& window, const InputEvent input)
{
	if (input == jumpTrigger)
	{
		if (onGround)
		{
			jump();
		}
		else
		{
			if (doubleJumpCharge)
			{
				doubleJumpCharge = false;
				jump();
			}
			// else {
			//	if(state == State::falling) queueJump = true;
			// }
		}
	}
}

void TripleNinePlayer::calculateHeadPosition(Window& window, const Float delta)
{
	Float eyeHeightTarget = eyeHeightNormal;
	// advance walk cycle
	walkCycle += actualSpeed * delta / unit;
	// height
	switch (state)
	{
	case State::Standing: eyeHeightTarget = eyeHeightNormal;
		break;
	case State::Walking: eyeHeightTarget = eyeHeightWalking;
		break;
	case State::Sprinting: eyeHeightTarget = eyeHeightRunning;
		break;
	case State::Falling: eyeHeightTarget = eyeHeightFlying;
		break;
	case State::Jumping: eyeHeightTarget = eyeHeightNormal;
		break;
	case State::Crouching: eyeHeightTarget = eyeHeightCrouching;
		break;
	case State::Proning: eyeHeightTarget = eyeHeightProning;
		break;
	case State::Sliding: eyeHeightTarget = eyeHeightSliding;
		break;
	case State::Sneaking: eyeHeightTarget = eyeHeightCrouching;
		break;
	case State::Crawling: eyeHeightTarget = eyeHeightSliding;
		break;
	}
	// bob
	Float bobTarget = 0;
	if (isMoving && onGround) bobTarget = sin(lerp(1, headBobSpeed, 0.5) * 2 * walkCycle) * actualSpeed * headBobIntensity;
	// sway
	// Float swayTarget = 0;
	// if(onGround) swayTarget = sin(lerp(1, headBobSpeed, 0.5) * walkCycle) * actualSpeed * headSwayImpact;
	// lean
	if (onGround)
	{
		fullBodyLeanControl = approach(2.0f * actualSpeed / unit * targetCameraRotation.x, 1.0f);
	}
	else
	{
		fullBodyLeanControl = 0;
	}
	// smooth motion
	approach(upperBodyLean, upperBodyLeanControl, delta * upperBodyLeanSpeed);
	approach(fullBodyLean, fullBodyLeanControl, delta * fullBodyLeanSpeed);
	approach(eyeHeight, eyeHeightTarget, delta * eyeMovementSpeed);
	// leanOffset based on lean angle
	upperBodyLeanAngle = upperBodyLean * upperBodyMaxLeanAngle;
	fullBodyLeanAngle  = fullBodyLean * fullBodyMaxLeanAngle;
	// full body lean offset up to chest height
	auto leanOffset = Vec2(lowerChestHeight * sin(fullBodyLeanAngle), lowerChestHeight * cos(fullBodyLeanAngle) - lowerChestHeight);
	// full body and upper body offset from chest to eye height
	const Float lowerChestToEyeDistance = eyeHeightNormal - lowerChestHeight;
	const Float fullLeanAngle           = fullBodyLeanAngle + upperBodyLeanAngle;
	leanOffset += Vec2(lowerChestToEyeDistance * sin(fullLeanAngle), lowerChestToEyeDistance * cos(fullLeanAngle) - lowerChestToEyeDistance);
	// apply changes to camera
	camera.setRotation(DVec3(camera.getRotationX(), -(fullBodyLeanAngle + upperBodyLean * upperBodyLeanHeadTiltFactor), camera.getRotationZ()));
	// calculate 3d position of all offsets
	const Vec3 sway3D       = -sway * normalize(camera.getRightVector() * Vec3(1, 1, 0));
	const Vec3 lean3D       = leanOffset.x * camera.getRightVector() + Vec3(0, 0, leanOffset.y);
	const auto headHeight3D = Vec3(0, 0, eyeHeight + bobTarget);
	camera.setPosition(position + sway3D + headHeight3D + lean3D);
}

// Renderer

void TripleNineRenderer::update(Window& window)
{
	client.processEvents();
	player.update(window);
	if (gameState.getPlayerPosition(client.getClientToken()) != player.position)
	{
		client.sendPlayerData(player.position, player.camera.getRotation(), gameState.getTick());
	}
	gameState.updatePlayer(client.getClientToken(), player.position);
}

void TripleNineRenderer::create(Window& window)
{
	client.create();

	Renderer& r = window.renderer;

	mainFramebuffer.create("MainFramebuffer", Area(1920, 1080));
	mainFramebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	mainFramebuffer.add(r, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	mainFramebuffer.checkComplete();

	bloom1Framebuffer.create("Bloom1Framebuffer", Area(1920, 1080));
	bloom1Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom1Framebuffer.checkComplete();

	bloom2Framebuffer.create("Bloom2Framebuffer", Area(1920, 1080));
	bloom2Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom2Framebuffer.checkComplete();

	bloom3Framebuffer.create("Bloom3Framebuffer", Area(1920, 1080));
	bloom3Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom3Framebuffer.checkComplete();

	bloom4Framebuffer.create("Bloom4Framebuffer", Area(1920, 1080));
	bloom4Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom4Framebuffer.checkComplete();

	bloom5Framebuffer.create("Bloom5Framebuffer", Area(1920, 1080));
	bloom5Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom5Framebuffer.checkComplete();

	bloom6Framebuffer.create("Bloom6Framebuffer", Area(1920, 1080));
	bloom6Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom6Framebuffer.checkComplete();

	bloom7Framebuffer.create("Bloom7Framebuffer", Area(1920, 1080));
	bloom7Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom7Framebuffer.checkComplete();

	bloom8Framebuffer.create("Bloom8Framebuffer", Area(1920, 1080));
	bloom8Framebuffer.add(r, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	bloom8Framebuffer.checkComplete();

	// set near and far clip distance for player
	player.camera.setNearClipValue(0.01f);
	player.camera.setFarClipValue(1000.0f);
}

void TripleNineRenderer::destroy(Window& window)
{
	bloom8Framebuffer.destroy(window.renderer);
	bloom7Framebuffer.destroy(window.renderer);
	bloom6Framebuffer.destroy(window.renderer);
	bloom5Framebuffer.destroy(window.renderer);
	bloom4Framebuffer.destroy(window.renderer);
	bloom3Framebuffer.destroy(window.renderer);
	bloom2Framebuffer.destroy(window.renderer);
	bloom1Framebuffer.destroy(window.renderer);
	mainFramebuffer.destroy(window.renderer);

	client.destroy();
}

void TripleNineRenderer::connect(GameSimulation& simulation) {}

void TripleNineRenderer::renderBloom(Renderer& r) const
{
	// setup rendering
	r.uniforms().reset();
	r.setDepthTest(false);
	r.setWireframeMode(false);
	r.setAlphaBlending(false);

	// prefilter (remove fireflies)
	mainFramebuffer.bindRead();
	mainFramebuffer.setAsTexture();
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomPrefilter");

	bloom1Framebuffer.bindRead();
	bloom1Framebuffer.setAsTexture();
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom2Framebuffer.bindRead();
	bloom2Framebuffer.setAsTexture();
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom3Framebuffer.bindRead();
	bloom3Framebuffer.setAsTexture();
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom4Framebuffer.bindRead();
	bloom4Framebuffer.setAsTexture();
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom5Framebuffer.bindRead();
	bloom5Framebuffer.setAsTexture();
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom6Framebuffer.bindRead();
	bloom6Framebuffer.setAsTexture();
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	bloom7Framebuffer.bindRead();
	bloom7Framebuffer.setAsTexture();
	bloom8Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomDownsample");

	r.blendModeAdditive();

	bloom8Framebuffer.bindRead();
	bloom8Framebuffer.setAsTexture();
	bloom7Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom7Framebuffer.bindRead();
	bloom7Framebuffer.setAsTexture();
	bloom6Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom6Framebuffer.bindRead();
	bloom6Framebuffer.setAsTexture();
	bloom5Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom5Framebuffer.bindRead();
	bloom5Framebuffer.setAsTexture();
	bloom4Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom4Framebuffer.bindRead();
	bloom4Framebuffer.setAsTexture();
	bloom3Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom3Framebuffer.bindRead();
	bloom3Framebuffer.setAsTexture();
	bloom2Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom2Framebuffer.bindRead();
	bloom2Framebuffer.setAsTexture();
	bloom1Framebuffer.bindDraw(r);
	r.fullScreenShader("bloomUpsample");

	bloom1Framebuffer.bindRead();
	bloom1Framebuffer.setAsTexture();
	mainFramebuffer.bindDraw(r);
	r.fullScreenShader("lastBloomUpsample");

	r.setAlphaBlending(false);
}

void TripleNineRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	// resize if necessary
	if (frameSize != area.size)
	{
		frameSize = area.size;
		mainFramebuffer.resize(r, frameSize);
		bloom1Framebuffer.resize(r, frameSize * pow(0.5f, 1.0f));
		bloom2Framebuffer.resize(r, frameSize * pow(0.5f, 2.0f));
		bloom3Framebuffer.resize(r, frameSize * pow(0.5f, 3.0f));
		bloom4Framebuffer.resize(r, frameSize * pow(0.5f, 4.0f));
		bloom5Framebuffer.resize(r, frameSize * pow(0.5f, 5.0f));
		bloom6Framebuffer.resize(r, frameSize * pow(0.5f, 6.0f));
		bloom7Framebuffer.resize(r, frameSize * pow(0.5f, 7.0f));
		bloom8Framebuffer.resize(r, frameSize * pow(0.5f, 8.0f));
	}

	// clear screen
	mainFramebuffer.clear();
	bloom1Framebuffer.clear();
	bloom2Framebuffer.clear();
	bloom3Framebuffer.clear();
	bloom4Framebuffer.clear();
	bloom5Framebuffer.clear();
	bloom6Framebuffer.clear();
	bloom7Framebuffer.clear();
	bloom8Framebuffer.clear();

	mainFramebuffer.bindDraw(r);

	// default everything
	r.uniforms().reset();
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(false);
	// render sky
	player.camera.renderOnlyRot(r);
	r.uniforms().setSunDir(Vec4(sunDirection, 1));
	r.useTexture("tripleNineSky");
	r.useShader("tripleNineSky");
	r.renderMesh("tripleNineSky");
	r.setDepthTest(true);
	r.setCulling(true);
	// render scene
	player.render(window);
	r.fill(Vec3(1));
	r.useShader("tripleNineMap");
	// r.useTexture("tripleNineReflection");

	if (renderMap)
	{
		r.uniforms().setMMatrix(matrixFromSize(mapSize));
		r.useTexture("Map1 HDR");
		r.renderMesh("Map1");
		r.useTexture("Map2 HDR");
		r.renderMesh("Map2");
		r.useTexture("Map3 HDR");
		r.renderMesh("Map3");
		r.useTexture("Map4 HDR");
		r.renderMesh("Map4");
		r.useTexture("Map5 HDR");
		r.renderMesh("Map5");
		r.useTexture("Map6 HDR");
		r.renderMesh("Map6");
		r.useTexture("Map7 HDR");
		r.renderMesh("Map7");
		r.useTexture("Map8 HDR");
		r.renderMesh("Map8");
		r.useTexture("Map9 HDR");
		r.renderMesh("Map9");
	}

	#ifdef TRIPLE_NINE_ENEMY
	r.useTexture("tripleNineTarget_baked");
	Vector<Matrix> enemies;
	for (const TripleNineEnemy& enemy : gameState.enemies)
	{
		enemies.push_back(matrixFromPosition(enemy.position));
	}
	r.renderMeshInstanced("tripleNineTarget", enemies);
	#endif

	// sphere
	r.useShader("color");
	r.uniforms().setCustomColor(Vec4(10000000));
	r.uniforms().setMMatrix(matrixFromPosition(Vec3(160, 50, 15 * pow(abs(sin(r.time.getTotal() * 2)), 0.5) + 1)));
	r.renderMesh("sphere");

	r.setDepthTest(false);

	// [TODO] reenable Bloom
	if (bloom) renderBloom(r);

	// crosshair
	r.uniforms().reset();
	r.screenSpace();
	r.fill(0.1f, 0.1f, 0.1f);
	r.useShader("color");
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 3));
	r.renderMesh("circle");
	r.fill(1, 1, 1);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 2));
	r.renderMesh("circle");

	if (renderText)
	{
		r.screenSpace();
		Int             i       = 1;
		constexpr Float spacing = 30.0f;
		r.uniforms().setMMatrix(Matrix(1));
		r.textRenderSystem.setSize(16.0f);
		r.textRenderSystem.setLetterSpacing(0.6f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		r.textRenderSystem.render(r, "on Ground: " + toString(player.onGround), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "is moving: " + toString(player.isMoving), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "full body lean: " + toString(player.fullBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "target id: " + toString(r.objectId), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "vertical speed: " + toString(player.velocity.z), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "double jump: " + toString(player.doubleJumpCharge), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "max jump height: " + toString(player.debugCurrentMaxJumpHeight), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "cameraForwardVector: " + toString(player.camera.getForwardVector()), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(r, "fps: " + toString(1.0f / r.time.getDelta()), Vec2(spacing, static_cast<Float>(i) * spacing));
	}

	// render to window
	r.setDepthTest(false);
	mainFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("tonemapping");

	if (vsync) limitFramerate(60);
}

void TripleNineRenderer::inputEvent(Window& window, const InputEvent input)
{
	if (input == enter)
	{
		if (!window.capturing)
		{
			window.captureCursor();
		}
		else
		{
			#ifdef TRIPLE_NINE_ENEMY
			//if (window.renderer.objectId != static_cast<UInt>(-1)) killEnemy(window.renderer.objectId);
			#endif
		}
	}
	// if (input == exit) window.releaseCursor();
	if (input == reloadShaders) window.renderer.shaderSystem.rebuild();
	if (input == toggleBloom) bloom = !bloom;
	player.inputEvent(window, input);
}

void TripleNineRenderer::renderInteractive(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.idFramebuffer.resize(r, area.size);
	r.idFramebuffer.clearColor(IVec4(0));
	r.idFramebuffer.clearDepth();
	r.idFramebuffer.bindDraw(r);
	r.setWireframeMode(false);
	r.setDepthTest(true);
	r.setCulling(false);
	r.setAlphaBlending(false);
	player.render(window);
	r.useShader("idShader");

	if (renderMap)
	{
		r.uniforms().setMMatrix(matrixFromSize(mapSize));
		r.renderMesh("Map1");
		r.renderMesh("Map2");
		r.renderMesh("Map3");
		r.renderMesh("Map4");
		r.renderMesh("Map5");
		r.renderMesh("Map6");
		r.renderMesh("Map7");
		r.renderMesh("Map8");
		r.renderMesh("Map9");
	}

	#ifdef TRIPLE_NINE_ENEMY
	r.uniforms().setMMatrix(Matrix(1));
	for (TripleNineEnemy& enemy : gameState.getEnemies())
	{
		r.uniforms().setMMatrix(matrixFromPositionAndSize(enemy.getPosition(), 1));
		r.uniforms().setObjectId(enemy.getId());
		r.renderMesh("tripleNineTarget");

		enemy.renderInteractive(window, area);
	}
	#endif

	r.idFramebuffer.bindRead();
	IVec4 idData;
	if (window.capturing)
	{
		idData = r.idFramebuffer.readPixelsAtCenterUIntRGB(FramebufferMode::Color0);
	}
	else
	{
		idData = r.idFramebuffer.readPixelsUIntRGB(static_cast<Int>(window.mousePosBotLeft.x), static_cast<Int>(window.mousePosBotLeft.y), FramebufferMode::Color0);
	}
	r.objectId    = idData.x;
	r.instanceId  = idData.y;
	r.primitiveId = idData.z;

	// render to window
	r.setDepthTest(false);
	r.idFramebuffer.bindRead();
	r.idFramebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("debugID");
}

// Server

void TripleNineServer::destroy()
{
	logDebug("Server: Shutting down...");
	disconnectClients();
	logDebug("Server: Shut down complete");
}

void TripleNineServer::sendMessage()
{
	outMessage.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		if (client.clientToken == outMessage.clientToken)
		{
			client.lastSent                                         = now();
			Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&outMessage);
			socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
			return;
		}
	}
}

void TripleNineServer::processEvents()
{
	Array<Char, sizeof(TripleNineMessage)> inMessageBuffer;
	while (socket.available())
	{
		socket.receive_from(asio::buffer(inMessageBuffer, sizeof(TripleNineMessage)), senderEndpoint);
		inMessage = *reinterpret_cast<TripleNineMessage*>(&inMessageBuffer);
		switch (inMessage.event.clientEvent)
		{
		case TripleNineClientToServerMessages::WantsToConnect: clientWantsToConnect();
			break;
		case TripleNineClientToServerMessages::SendPlayerData: broadcastPlayerData();
			break;
		case TripleNineClientToServerMessages::WantsToDisconnect: disconnectClient();
			break;
		case TripleNineClientToServerMessages::IsAlive: clientIsAlive();
			break;
		default: logWarning("Unknown event!");
			break;
		}
	}

	// remove dead clients
	for (Int i = 0; i < clients.size(); i++)
	{
		if (now() - clients[i].lastReceived > deadClientTimeout)
		{
			logDebug("Server: Client (" + toString(clients[i].clientToken) + ") timed out");
			clients.erase(clients.begin() + i);
			i--;
		}
	}

	//send keep-alive message
	if (now() - lastPingSent > pingInterval)
	{
		lastPingSent                 = now();
		outMessage.clientToken       = 0;
		outMessage.event.serverEvent = TripleNineServerToClientMessages::CheckIsAlive;
		broadcastMessage();
	}
}

void TripleNineServer::broadcastPlayerData()
{
	UShort currentClient = inMessage.clientToken;

	for (TripleNineClientInfo& client : clients)
	{
		if (client.clientToken == currentClient)
		{
			client.playerData.position = inMessage.position;
			client.playerData.rotation = inMessage.rotation;
			continue;
		}
		outMessage.event.serverEvent = TripleNineServerToClientMessages::BroadcastPlayerData;
		outMessage.clientToken       = client.clientToken;
		outMessage.position          = inMessage.position;
		outMessage.rotation          = inMessage.rotation;
		sendMessage();
	}
}

void TripleNineServer::disconnectClients()
{
	logDebug("Server: Disconnecting clients...");
	for (TripleNineClientInfo& client : clients)
	{
		logDebug("Server: Disconnecting client (" + toString(client.clientToken) + ")");
		outMessage.clientToken       = client.clientToken;
		outMessage.event.serverEvent = TripleNineServerToClientMessages::BroadcastDisconnectClient;
		broadcastMessage();
	}
	clients.clear();
}

void TripleNineServer::broadcastMessage()
{
	outMessage.timeStamp = now();
	for (TripleNineClientInfo& client : clients)
	{
		client.lastSent                                         = now();
		Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&outMessage);
		socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
	}
}

void TripleNineServer::disconnectClient()
{
	logDebug("Server: Client (" + toString(inMessage.clientToken) + ") disconnecting");
	for (Int i = 0; i < clients.size(); i++)
	{
		if (clients[i].clientToken == inMessage.clientToken)
		{
			clients.erase(clients.begin() + i);
		}
	}
}

void TripleNineServer::clientIsAlive()
{
	//calculate ping

	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			client.lastReceived = now();
		}
	}
}

void TripleNineServer::create() const
{
	logDebug("Server: Starting...");
}

void TripleNineServer::clientWantsToConnect()
{
	logDebug("Server: Registering new client...");

	if (inMessage.clientToken != 0) logDebug("Server: Warning: Client already has id");

	logDebug(senderEndpoint.address().to_string() + ":" + toString(senderEndpoint.port()));

	for (TripleNineClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			//send him his already existing id
			outMessage.event.serverEvent = TripleNineServerToClientMessages::ConfirmConnection;
			outMessage.clientToken       = client.clientToken;
			logDebug("Server: Client (" + toString(outMessage.clientToken) + ") already exists, sending again");
			sendMessage();
			return;
		}
	}

	const Token newToken = generateRandomToken();

	// store info
	TripleNineClientInfo info;
	info.clientToken = newToken;
	info.endpoint    = senderEndpoint;
	clients.push_back(info);

	// give client its new id
	outMessage.event.serverEvent = TripleNineServerToClientMessages::ConfirmConnection;
	outMessage.clientToken       = newToken;
	logDebug("Server: New client registered");

	logDebug("Server: Client assigned id: " + toString(outMessage.clientToken));
	sendMessage();
}

// Client

void TripleNineClient::disconnectFromServer()
{
	logDebug("Client (" + toString(myToken) + "): Disconnecting...");
	outMessage.event.clientEvent = TripleNineClientToServerMessages::WantsToDisconnect;
	outMessage.clientToken       = myToken;
	sendMessage();
	connected = false;
}

void TripleNineClient::create()
{
	serverEndpoint = *UDPResolver(context).resolve(asio::ip::udp::v4(), host, toString(port)).begin();
	logDebug(serverEndpoint.address().to_string() + ":" + toString(serverEndpoint.port()));
	socket.open(asio::ip::udp::v4());
	lastReceived = now();
}

void TripleNineClient::destroy()
{
	disconnectFromServer();
}

void TripleNineClient::connectToServer()
{
	logDebug("Client (" + toString(myToken) + "): Client trying to connect...");
	outMessage.event.clientEvent = TripleNineClientToServerMessages::WantsToConnect;
	outMessage.clientToken       = myToken;
	sendMessage();
	Sleep(1000);
}

void TripleNineClient::connectionDeclined()
{
	logDebug("Client (" + toString(myToken) + "): Server refused connection");
	connected = false;
}

void TripleNineClient::playerDataReceived()
{
	// update player data
	for (TripleNineClientInfo& client : clients)
	{
		if (client.clientToken == inMessage.clientToken)
		{
			client.playerData.position = inMessage.position;
			client.playerData.rotation = inMessage.rotation;
			return;
		}
	}

	// must be new client
	TripleNineClientInfo newClient;
	newClient.clientToken         = inMessage.clientToken;
	newClient.playerData.position = inMessage.position;
	newClient.playerData.rotation = inMessage.rotation;
	clients.push_back(newClient);
}

void TripleNineClient::playerDisconnected()
{
	if (inMessage.clientToken == myToken)
	{
		// disconnect
		connected = false;
		logDebug("Client (" + toString(myToken) + "): Server requested disconnect!");
	}
	else
	{
		for (Int i = 0; i < clients.size(); i++)
		{
			// [TODO] remove player
			if (clients[i].clientToken == inMessage.clientToken)
			{
				clients.erase(clients.begin() + i);
				logDebug("Client (" + toString(myToken) + "): Player (" + toString(inMessage.clientToken) + ") disconnected");
				return;
			}
		}
	}
}

void TripleNineClient::connectionConfirmed()
{
	logDebug("Client (" + toString(myToken) + "): Server confirmed connection");
	logDebug("Client (" + toString(myToken) + "): Server requested ID change");
	this->myToken = inMessage.clientToken;
	logDebug("Client (" + toString(myToken) + "): Client was assigned ID:" + toString(myToken));
	if (!connected)
	{
		logDebug("Client (" + toString(myToken) + "): Connected...");
		connected = true;
	}
}

Token TripleNineClient::getClientToken() const
{
	return myToken;
}

void TripleNineClient::processEvents()
{
	if (!connected) connectToServer();

	while (socket.available())
	{
		Array<Char, sizeof(TripleNineMessage)> inMessageBuffer;

		socket.receive_from(asio::buffer(inMessageBuffer, sizeof(TripleNineMessage)), serverEndpoint);
		inMessage = *reinterpret_cast<TripleNineMessage*>(&inMessageBuffer);

		switch (inMessage.event.serverEvent)
		{
		case TripleNineServerToClientMessages::DeclineConnection: connectionDeclined();
			break;
		case TripleNineServerToClientMessages::ConfirmConnection: connectionConfirmed();
			break;
		case TripleNineServerToClientMessages::BroadcastPlayerData: playerDataReceived();
			break;
		case TripleNineServerToClientMessages::BroadcastDisconnectClient: playerDisconnected();
			break;
		case TripleNineServerToClientMessages::CheckIsAlive: respondToPing();
			break;
		default: logWarning("Unknown event!");
			break;
		}
	}
}

void TripleNineClient::sendPlayerData(const Vec3 position, const Vec3 rotation, const ULL gameTick)
{
	outMessage.timeStamp         = now();
	outMessage.clientToken       = myToken;
	outMessage.position          = position;
	outMessage.rotation          = rotation;
	outMessage.tick              = gameTick;
	outMessage.event.clientEvent = TripleNineClientToServerMessages::SendPlayerData;
	sendMessage();
}

void TripleNineClient::waitTillConnected() const
{
	while (!connected) {}
}

void TripleNineClient::sendMessage()
{
	outMessage.timeStamp                                    = now();
	lastSent                                                = now();
	Array<Char, sizeof(TripleNineMessage)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(TripleNineMessage)>*>(&outMessage);
	socket.send_to(asio::buffer(outMessageBuffer), serverEndpoint);
}

void TripleNineClient::respondToPing()
{
	logDebug("Client (" + toString(myToken) + "): I am alive");
	outMessage.event.clientEvent = TripleNineClientToServerMessages::IsAlive;
	outMessage.clientToken       = myToken;
	sendMessage();
}

// Game

void TripleNine::run()
{
	simulation.start();

	if (useIntro)
	{
		intro.init(renderer1);
		ui.window("Triple Nine", Area(1920, 1080), true, false, WindowState::Windowed, intro);
	}
	else
	{
		ui.window("Triple Nine", Area(1920, 1080), true, true, WindowState::Windowed, renderer1);

		Sleep(3000);

		ui.window("Triple Nine", Area(1920, 1080), true, true, WindowState::Windowed, renderer2);
	}

	ui.run();
	simulation.stop();
}
