#include "TripleNine.hpp"
#include "Window.hpp"

void TripleNine::run()
{
	resourceManager.create();
	simulation.create(resourceManager);
	renderer.connect(simulation);
	ui.create();
	intro.init(renderer);
	ui.window("Triple Nine", Area(1920, 1080), true, false, WindowState::Windowed, intro, resourceManager);
	ui.run();
}

void TripleNineEnemy::die()
{
	position = Vec3(randomVec2Fast(-85.0f, 85.0f), 0.0f);
}

void TripleNinePlayer::jump()
{
	if(isMoving)
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
	state    = State::jumping;
}

void TripleNineSimulation::destroy() {}

void TripleNineEnemy::update(Window& window) {}

void TripleNineEnemy::destroy(Window& window) {}

void TripleNineSimulation::update(Float delta) {}

void TripleNineSimulation::createEnemy()
{
	enemies.emplace_back();
	enemies.back().id = static_cast<Index>(enemies.size()) - static_cast<Index>(1);
}

void TripleNinePlayer::collisionResponse()
{
	if(velocity.z < 0) velocity.z = 0;
	if(location.z < 0) location.z = 0;
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
	if(window.capturing) targetCameraRotation += window.mouseDelta * MouseMovement(mouseSensitivity);
	movementControl = Vec3(0);
	if(window.pressed(forward)) movementControl += forwardVector;
	if(window.pressed(backward)) movementControl -= forwardVector;
	if(window.pressed(right)) movementControl += rightVector;
	if(window.pressed(left)) movementControl -= rightVector;
	upperBodyLeanControl = 0;
	if(window.pressed(leanRight) && onGround) upperBodyLeanControl += 1;
	if(window.pressed(leanLeft) && onGround) upperBodyLeanControl -= 1;
	movementInput = movementControl != Vec3(0);
	if(onGround)
	{
		if(window.pressed(crouch))
		{
			if(window.pressed(holdJump))
			{
				if(movementInput) state = State::crawling;
				else state              = State::proning;
			}
			else
			{
				if(movementInput) state = State::sneaking;
				else state              = State::crouching;
			}
		}
		else
		{
			if(movementInput)
			{
				if(window.pressed(run)) state = State::sprinting;
				else state                    = State::walking;
			}
			else state = State::standing;
		}
	}
	else
	{
		if(velocity.z >= 0) state = State::jumping;
		else state                = State::falling;
	}
	// update values
	// running
	if(state == State::sprinting) speedControl += delta * speedControlAcceleration;
	else speedControl -= delta * speedControlDeceleration;
	speedControl = clamp(speedControl, 0, 1);
	camera.rotate(targetCameraRotation);
	calculatePhysics(window);
	if(isCollidingWithGround())
	{
		onGround         = true;
		doubleJumpCharge = true;
		if(window.pressed(holdJump)) jump();
		// if (queueJump) {
		//	queueJump = false;
		// }
		collisionResponse();
		if(movementInput) actualFriction = movementFriction;
		else actualFriction              = stopFriction;
		actualFriction *= normalFrictionFactor;
		velocity /= 1 + delta * actualFriction;
	}
	else onGround = false;
	if(state == State::jumping) debugCurrentMaxJumpHeight = location.z;
	isMoving = movementControl != Vec3(0);
	// air strafing
	if(movementInput && !onGround)
	{
		const Vec3  strafeVector     = movementControl * Vec3(maxStrafeSpeed);
		const Vec3  projectionVector = proj(velocity, movementControl);
		const Vec3  deltaVec         = strafeVector - projectionVector;
		const Float velocityAddMax   = length(deltaVec);
		if(length(projectionVector) < velocityAddMax) velocity += movementControl * min(delta * maxStrafeAcceleration, velocityAddMax);
	}
	// movement speed
	targetSpeed = lerp(walkingSpeed, runningSpeed, speedControl);
	if(movementInput && onGround)
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

void TripleNineRenderer::update(Window& window)
{
	player.update(window);
}

void TripleNineRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

Bool TripleNinePlayer::isCollidingWithGround() const
{
	return location.z <= 0;
}

void TripleNineRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<TripleNineSimulation*>(&simulation);
}

void TripleNineEnemy::inputEvent(Window& window, InputEvent input) {}

void TripleNinePlayer::calculatePhysics(const Window& window)
{
	// step 1
	const Vec3 velocity1 = velocity + acceleration * window.renderer.deltaTime() / Vec3(2);
	// step 2
	location += velocity1 * window.renderer.deltaTime();
	// step 3 calculate forces
	// player keeps jumping higher when holding space but falls faster when releasing it
	if(!onGround && window.pressed(holdJump)) acceleration = Vec3(0, 0, -airTimeGravity);
	else acceleration                                      = Vec3(0, 0, -gravity);
	// step4
	const Vec3 velocity2 = velocity1 + acceleration * window.renderer.deltaTime() / Vec3(2);
	velocity             = velocity2 * pow(airResistance, window.renderer.deltaTime());
}

void TripleNineSimulation::create(ResourceManager& resourceManager)
{
	// create targets
	for(Int i = 0; i < 100; i++) createEnemy();
}

void TripleNineEnemy::create(ResourceManager& resourceManager, Window& window) {}

void TripleNinePlayer::inputEvent(Window& window, const InputEvent input)
{
	if(input == jumpTrigger)
	{
		if(onGround) jump();
		else
		{
			if(doubleJumpCharge)
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

void TripleNineRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter)
	{
		if(!window.capturing) window.captureCursor();
		else
		{
			if(window.renderer.objectId != static_cast<UInt>(-1)) sim->enemies[window.renderer.objectId].die();
		}
	}
	if(input == exit) window.releaseCursor();
	if(input == reloadShaders) window.renderer.shaderSystem.rebuild();
	if(input == toggleBloom) bloom = !bloom;
	player.inputEvent(window, input);
}

void TripleNinePlayer::calculateHeadPosition(Window& window, const Float delta)
{
	Float eyeHeightTarget = eyeHeightNormal;
	// advance walk cycle
	walkCycle += actualSpeed * delta / unit;
	// height
	switch(state)
	{
		case State::standing:
			eyeHeightTarget = eyeHeightNormal;
			break;
		case State::walking:
			eyeHeightTarget = eyeHeightWalking;
			break;
		case State::sprinting:
			eyeHeightTarget = eyeHeightRunning;
			break;
		case State::falling:
			eyeHeightTarget = eyeHeightFlying;
			break;
		case State::jumping:
			eyeHeightTarget = eyeHeightNormal;
			break;
		case State::crouching:
			eyeHeightTarget = eyeHeightCrouching;
			break;
		case State::proning:
			eyeHeightTarget = eyeHeightProning;
			break;
		case State::sliding:
			eyeHeightTarget = eyeHeightSliding;
			break;
		case State::sneaking:
			eyeHeightTarget = eyeHeightCrouching;
			break;
		case State::crawling:
			eyeHeightTarget = eyeHeightSliding;
			break;
	}
	// bob
	Float bobTarget = 0;
	if(isMoving && onGround) bobTarget = sin(lerp(1, headBobSpeed, 0.5) * 2 * walkCycle) * actualSpeed * headBobIntensity;
	// sway
	// Float swayTarget = 0;
	// if(onGround) swayTarget = sin(lerp(1, headBobSpeed, 0.5) * walkCycle) * actualSpeed * headSwayImpact;
	// lean
	if(onGround) fullBodyLeanControl = approach(2.0f * actualSpeed / unit * targetCameraRotation.x, 1.0f);
	else fullBodyLeanControl         = 0;
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
	camera.setLocation(location + sway3D + headHeight3D + lean3D);
}

void TripleNineRenderer::create(ResourceManager& resourceManager, Window& window)
{
	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();

	// [TODO] replace
	// fs.addFramebuffer("bloom1", pow(0.5, 1));
	// fs.addFramebuffer("bloom2", pow(0.5, 2));
	// fs.addFramebuffer("bloom3", pow(0.5, 3));
	// fs.addFramebuffer("bloom4", pow(0.5, 4));
	// fs.addFramebuffer("bloom5", pow(0.5, 5));
	// fs.addFramebuffer("bloom6", pow(0.5, 6));
	// fs.addFramebuffer("bloom7", pow(0.5, 7));
	// fs.addFramebuffer("bloom8", pow(0.5, 8));
	// fs.addFramebuffer("bloom9", pow(0.5, 9));

	// set near and far clip distance for player
	player.camera.setNearClipValue(0.01f);
	player.camera.setFarClipValue(1000.0f);
}

void TripleNineEnemy::render(ResourceManager& resourceManager, Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(position, 1));
	renderer.renderMesh(resourceManager, "tripleNineTarget");
}

void TripleNineRenderer::renderBloom(ResourceManager& resourceManager, Renderer& r) const
{
	//	FramebufferSystem& fs = r.framebufferSystem;
	//	// setup rendering
	//	r.setDepthTest(false);
	//	r.setWireframeMode(false);
	//	r.setAlphaBlending(false);
	//	// prefilter (remove fireflies)
	//	r.read("main");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom1");
	//	r.fullScreenShader(resourceManager, "bloomPrefilter");
	//	// downsample pass
	//	// r.read("postProcess");
	//	// fs.currentRead().setAsTexture();
	//	// r.draw("bloom1");
	//	// r.fullScreenShader(e, "bloomDownsample");
	//	r.read("bloom1");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom2");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom2");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom3");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom3");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom4");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom4");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom5");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom5");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom6");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom6");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom7");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.read("bloom7");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom8");
	//	r.fullScreenShader(resourceManager, "bloomDownsample");
	//	r.blendModeAdditive();
	//	r.read("bloom8");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom7");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom7");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom6");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom6");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom5");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom5");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom4");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom4");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom3");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom3");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom2");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom2");
	//	fs.currentRead().setAsTexture();
	//	r.draw("bloom1");
	//	r.fullScreenShader(resourceManager, "bloomUpsample");
	//	r.read("bloom1");
	//	fs.currentRead().setAsTexture();
	//	r.draw("main");
	//	r.fullScreenShader(resourceManager, "lastBloomUpsample");
	//	// r.bindRead("postProcess");
	//	// fs.currentRead().setAsTexture();
	//	// r.draw("main");
	//	// r.fullScreenShader(e, "lastBloomUpsample");
	//	r.setAlphaBlending(false);
}

void TripleNineEnemy::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void TripleNineRenderer::render(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// default everything
	r.uniforms().reset();
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(false);
	// render sky
	player.camera.renderOnlyRot(r);
	r.uniforms().setSunDir(Vec4(sunDirection, 1));
	r.useTexture(resourceManager, "tripleNineSky");
	r.useShader(resourceManager, "tripleNineSky");
	r.renderMesh(resourceManager, "tripleNineSky");
	r.setDepthTest(true);
	r.setCulling(true);
	// render scene
	player.render(resourceManager, window);
	r.fill(Vec3(1));
	r.useShader(resourceManager, "tripleNineMap");
	// r.useTexture(resourceManager, "tripleNineReflection");
	r.uniforms().setMMatrix(matrixFromSize(mapSize));
	r.useTexture(resourceManager, "Map1 HDR");
	r.renderMesh(resourceManager, "Map1");
	r.useTexture(resourceManager, "Map2 HDR");
	r.renderMesh(resourceManager, "Map2");
	r.useTexture(resourceManager, "Map3 HDR");
	r.renderMesh(resourceManager, "Map3");
	r.useTexture(resourceManager, "Map4 HDR");
	r.renderMesh(resourceManager, "Map4");
	r.useTexture(resourceManager, "Map5 HDR");
	r.renderMesh(resourceManager, "Map5");
	r.useTexture(resourceManager, "Map6 HDR");
	r.renderMesh(resourceManager, "Map6");
	r.useTexture(resourceManager, "Map7 HDR");
	r.renderMesh(resourceManager, "Map7");
	r.useTexture(resourceManager, "Map8 HDR");
	r.renderMesh(resourceManager, "Map8");
	r.useTexture(resourceManager, "Map9 HDR");
	r.renderMesh(resourceManager, "Map9");

	r.useTexture(resourceManager, "tripleNineTarget_baked");
	for(TripleNineEnemy& enemy : sim->enemies) enemy.render(resourceManager, r);

	// sphere
	r.useShader(resourceManager, "color");
	r.uniforms().setCustomColor(Vec4(1000000));
	r.uniforms().setMMatrix(matrixFromPosition(Vec3(160, 50, 15 * pow(abs(sin(r.time.total * 2)), 0.5) + 1)));
	r.renderMesh(resourceManager, "sphere");

	r.setDepthTest(false);

	// [TODO] reenable Bloom
	// if(bloom) renderBloom(resourceManager, r);

	// crosshair
	r.uniforms().reset();
	r.screenSpace();
	r.fill(0.1f, 0.1f, 0.1f);
	r.useShader(resourceManager, "color");
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 3));
	r.renderMesh(resourceManager, "circle");
	r.fill(1, 1, 1);
	r.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(window.getFrameSize() / 2), 2));
	r.renderMesh(resourceManager, "circle");

	if(renderText)
	{
		r.screenSpace();
		Int             i       = 1;
		constexpr Float spacing = 30.0f;
		r.uniforms().setMMatrix(Matrix(1));
		r.textRenderSystem.setSize(16.0f);
		r.textRenderSystem.setLetterSpacing(0.6f);
		r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
		r.textRenderSystem.render(resourceManager, r, "on Ground: " + toString(player.onGround), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "is moving: " + toString(player.isMoving), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "full body lean: " + toString(player.fullBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "target id: " + toString(r.objectId), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "vertical speed: " + toString(player.velocity.z), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "double jump: " + toString(player.doubleJumpCharge), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "max jump height: " + toString(player.debugCurrentMaxJumpHeight), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "cameraForwardVector: " + toString(player.camera.getForwardVector()), Vec2(spacing, static_cast<Float>(i++) * spacing));
		r.textRenderSystem.render(resourceManager, r, "fps: " + toString(1.0f / r.time.delta), Vec2(spacing, static_cast<Float>(i) * spacing));
	}

	// render to window
	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader(resourceManager, "tonemapping");
}

void TripleNineEnemy::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(position, 1));
	window.renderer.uniforms().setObjectId(id);
	window.renderer.renderMesh(resourceManager, "tripleNineTarget");
}

void TripleNineRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.idFramebuffer.resize(area.size);
	r.idFramebuffer.clearColor(IVec4(-1));
	r.idFramebuffer.clearDepth();
	r.idFramebuffer.bindDraw();
	r.setWireframeMode(false);
	r.setDepthTest(true);
	r.setCulling(false);
	r.setAlphaBlending(false);
	player.render(resourceManager, window);
	r.useShader(resourceManager, "idShader");
	r.uniforms().setMMatrix(matrixFromSize(mapSize));
	r.renderMesh(resourceManager, "Map1");
	r.renderMesh(resourceManager, "Map2");
	r.renderMesh(resourceManager, "Map3");
	r.renderMesh(resourceManager, "Map4");
	r.renderMesh(resourceManager, "Map5");
	r.renderMesh(resourceManager, "Map6");
	r.renderMesh(resourceManager, "Map7");
	r.renderMesh(resourceManager, "Map8");
	r.renderMesh(resourceManager, "Map9");
	r.uniforms().setMMatrix(Matrix(1));
	for(TripleNineEnemy& enemy : sim->enemies) enemy.renderInteractive(resourceManager, window, area);

	r.idFramebuffer.bindRead();
	IVec4 idData;
	if(window.capturing) idData = r.idFramebuffer.readPixelsAtCenterUIntRgb(FramebufferMode::Color0);
	else idData                 = r.idFramebuffer.readPixelsUIntRgb(static_cast<Int>(window.mousePosBotLeft.x), static_cast<Int>(window.mousePosBotLeft.y), FramebufferMode::Color0);
	r.objectId    = idData.x;
	r.instanceId  = idData.y;
	r.primitiveId = idData.z;

	// render to window
	// r.setDepthTest(false);
	// idFramebuffer.setAsTexture(0);
	// r.drawToWindow();
	// r.fullScreenShader(resourceManager, "debugID");
}
