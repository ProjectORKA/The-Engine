#include "TripleNine.hpp"
#include "Window.hpp"
#include "Debug.hpp"

Index enemyID = 0;
void TripleNinePlayer::jump() {
	if (isMoving) {
		velocity.z = 0;
		velocity += Vec3(0, 0, minJumpVelocity);
	}
	else {
		velocity.z = 0;
		velocity += Vec3(0, 0, lerp(minJumpVelocity, maxJumpVelocity, jumpCharge));
		jumpCharge = 0;
	}
	//queueJump = false;
	onGround = false;
	state = State::jumping;
}
void TripleNinePlayer::collisionResponse() {
	if (velocity.z < 0) velocity.z = 0;
	if (location.z < 0) location.z = 0;
}
Bool TripleNinePlayer::isCollidingWithGround() {
	return location.z <= 0;
}
void TripleNinePlayer::update(Window& window) {
	//set up temporary data
	Renderer& renderer = window.renderer;
	Float delta = renderer.deltaTime();

	//calculate player vectors
	forwardVector = normalize(Vec3(sin(camera.getRotationZ()), cos(camera.getRotationZ()), 0));
	rightVector = Vec3(forwardVector.y, -forwardVector.x, 0);

	//process input
	if (window.capturing)targetCameraRotation += window.mouseDelta * MouseMovement(mouseSensitivity);

	movementControl = Vec3(0);
	if (window.pressed(forward)) movementControl += forwardVector;
	if (window.pressed(backward)) movementControl -= forwardVector;
	if (window.pressed(right)) movementControl += rightVector;
	if (window.pressed(left)) movementControl -= rightVector;

	upperBodyLeanControl = 0;
	if (window.pressed(leanRight) && onGround) upperBodyLeanControl += 1;
	if (window.pressed(leanLeft) && onGround) upperBodyLeanControl -= 1;
	
	movementInput = movementControl != Vec3(0);

	if (onGround) {
		if (window.pressed(crouch))
		{
			if (window.pressed(holdJump))
			{
				if (movementInput)
				{
					state = State::crawling;
				}
				else
				{
					state = State::proning;
				}
			}
			else {
				if (movementInput)
				{
					state = State::sneaking;
				}
				else
				{
					state = State::crouching;
				}
			}
		}
		else
		{
			if (movementInput)
			{
				if (window.pressed(run))
				{
					state = State::sprinting;
				}
				else
				{
					state = State::walking;
				}
			}
			else
			{
				state = State::standing;
			}
		}
	}
	else
	{
		if (velocity.z >= 0)
		{
			state = State::jumping;
		}
		else
		{
			state = State::falling;
		}
	}


	//update values
		//running
	if (state == State::sprinting) speedControl += delta * speedControlAcceleration;
	else speedControl -= delta * speedControlDeceleration;

	speedControl = clamp(speedControl, 0, 1);

	camera.rotate(targetCameraRotation);

	calculatePhysics(window);

	if (isCollidingWithGround()) {
		onGround = true;
		doubleJumpCharge = true;

		if (window.pressed(holdJump)){
			jump();
		}

		//if (queueJump) {
		//	queueJump = false;
		//}

		collisionResponse();
		if (movementInput) actualFriction = movementFriction;
		else actualFriction = stopFriction;
		actualFriction *= normalFrictionFactor;
		velocity /= 1 + delta * actualFriction;

	}
	else onGround = false;

	if (state == State::jumping) debugCurrentMaxJumpHeight = location.z;

	isMoving = movementControl != Vec3(0);

	//air strafing
	if (movementInput && !onGround) {
		Vec3 strafeVector = movementControl * Vec3(maxStrafeSpeed);
		Vec3 projectionVector = proj(velocity, movementControl);
		Vec3 deltaVec = strafeVector - projectionVector;
		Float velocityAddMax = length(deltaVec);
		if (length(projectionVector) < velocityAddMax) {
			velocity += movementControl * min(delta * maxStrafeAcceleration, velocityAddMax);
		}
	}

	//movement speed
	targetSpeed = lerp(walkingSpeed, runningSpeed, speedControl);
	if (movementInput && onGround) {
		//add speed to direction
		approach(actualInputSpeed, targetSpeed, delta * 2);
		movementControl = normalize(movementControl);
		movementControl *= actualInputSpeed;

		approach(velocity.x, movementControl.x, 10 * delta);
		approach(velocity.y, movementControl.y, 10 * delta);
		movementControl = { 0, 0, 0 };
	}
	actualSpeed = length(velocity);

	//head position
	calculateHeadPosition(window, delta);

	//reset delta
	targetCameraRotation = DVec2(0);
}
void TripleNinePlayer::calculatePhysics(Window& window) {
	//step 1
	Vec3 velocity1 = velocity + acceleration * window.renderer.deltaTime() / Vec3(2);

	//step 2
	location += velocity1 * window.renderer.deltaTime();

	//step 3 calculate forces
	//player keeps jumping higher when holding space but falls faster when releasing it
	if (!onGround && window.pressed(holdJump)) acceleration = Vec3(0, 0, -airTimeGravity);
	else acceleration = Vec3(0, 0, -gravity);

	//step4
	Vec3 velocity2 = velocity1 + acceleration * window.renderer.deltaTime() / Vec3(2);

	velocity = velocity2 * pow(airResistance, window.renderer.deltaTime());
}
void TripleNinePlayer::inputEvent(Window& window, InputEvent input) {
	if (input == jumpTrigger) {
		if (onGround) {
			jump();
		} else {
			if (doubleJumpCharge) {
				doubleJumpCharge = false;
				jump();
			}
			//else {
			//	if(state == State::falling) queueJump = true;
			//}
		}
	}
}
void TripleNinePlayer::calculateHeadPosition(Window& window, Float delta) {

	Float eyeHeightTarget = eyeHeightNormal;

	//advance walk cycle
	walkCycle += actualSpeed * delta / unit;

	//height
	
	switch (state) {
		case State::standing:	eyeHeightTarget = eyeHeightNormal; break;
		case State::walking:	eyeHeightTarget = eyeHeightWalking; break;
		case State::sprinting:	eyeHeightTarget = eyeHeightRunning; break;
		case State::falling:	eyeHeightTarget = eyeHeightFlying; break;
		case State::jumping:	eyeHeightTarget = eyeHeightNormal; break;
		case State::crouching:	eyeHeightTarget = eyeHeightCrouching; break;
		case State::proning:	eyeHeightTarget = eyeHeightProning; break;
		case State::sliding:	eyeHeightTarget = eyeHeightSliding; break;
	}

	//bob
	Float bobTarget = 0;
	if (isMoving && onGround) bobTarget = sin(lerp(1, headBobSpeed, 0.5) * 2 * walkCycle) * actualSpeed * headBobIntensity;

	//sway
	Float swayTarget = 0;
	if (onGround) swayTarget = sin(lerp(1, headBobSpeed, 0.5) * walkCycle) * actualSpeed * headSwayImpact;

	//lean
	if (onGround) fullBodyLeanControl = approach(2 * actualSpeed / unit * targetCameraRotation.x, 1);
	else fullBodyLeanControl = 0;

	//smooth motion
	approach(upperBodyLean, upperBodyLeanControl, delta * upperBodyLeanSpeed);
	approach(fullBodyLean, fullBodyLeanControl, delta * fullBodyLeanSpeed);

	approach(eyeHeight, eyeHeightTarget, delta * eyeMovementSpeed);

	//leanOffset based on lean angle
	upperBodyLeanAngle = upperBodyLean * upperBodyMaxLeanAngle;
	fullBodyLeanAngle = fullBodyLean * fullBodyMaxLeanAngle;

	//full body lean offset up to chest height
	Vec2 leanOffset = Vec2(lowerChestHeight * sin(fullBodyLeanAngle), lowerChestHeight * cos(fullBodyLeanAngle) - lowerChestHeight);

	//full body and upper body offset from chest to eye height
	Float lowerChestToEyeDistance = eyeHeightNormal - lowerChestHeight;
	Float fullLeanAngle = fullBodyLeanAngle + upperBodyLeanAngle;
	leanOffset += Vec2(lowerChestToEyeDistance * sin(fullLeanAngle), lowerChestToEyeDistance * cos(fullLeanAngle) - lowerChestToEyeDistance);

	//apply changes to camera
	camera.setRotation(DVec3(camera.getRotationX(), -(fullBodyLeanAngle + upperBodyLean * upperBodyLeanHeadTiltFactor), camera.getRotationZ()));

	//calculate 3d position of all offsets
	Vec3 sway3D = -sway * normalize(camera.rightVector * Vec3(1, 1, 0));
	Vec3 lean3D = leanOffset.x * camera.rightVector + Vec3(0, 0, leanOffset.y);
	Vec3 headHeight3D = Vec3(0, 0, eyeHeight + bobTarget);

	camera.location = location + sway3D + headHeight3D + lean3D;
}

void TripleNineSimulation::create() {
	//create targets
	for (Int i = 0; i < 100; i++) {
		createEnemy();
	}
}

void TripleNineSimulation::createEnemy() {
	enemies.emplaceBack();
	enemies.last().id = enemies.size() - 1;
}

void TripleNineRenderer::connect(TripleNineSimulation& sim) {
	this->sim = &sim;
}

void TripleNineRenderer::update(Window& window) {
	player.update(window);
}
void TripleNineRenderer::create(ResourceManager& resourceManager, Window& window) {
	
	Renderer& r = window.renderer;
	FramebufferSystem& fs = window.renderer.framebufferSystem;
	
	//create framebuffers for bloom
	fs.addFrameBuffer("bloom1", pow(0.5, 1));
	fs.addFrameBuffer("bloom2", pow(0.5, 2));
	fs.addFrameBuffer("bloom3", pow(0.5, 3));
	fs.addFrameBuffer("bloom4", pow(0.5, 4));
	fs.addFrameBuffer("bloom5", pow(0.5, 5));
	fs.addFrameBuffer("bloom6", pow(0.5, 6));
	fs.addFrameBuffer("bloom7", pow(0.5, 7));
	fs.addFrameBuffer("bloom8", pow(0.5, 8));
	fs.addFrameBuffer("bloom9", pow(0.5, 9));

	//set near and far clip distance for player
	player.camera.nearClipValue = 0.01;
	player.camera.farClipValue = 1000.0;
}
void TripleNineRenderer::renderBloom(ResourceManager & resourceManager, Renderer & r) {
	FramebufferSystem& fs = r.framebufferSystem;

	//setup rendering
	r.setDepthTest(false);
	r.setWireframeMode(false);
	r.setAlphaBlending(false);

	//prefilter (remove fireflies)
	r.read("main");
	fs.currentRead().setAsTexture();
	r.draw("bloom1");
	r.fullScreenShader(resourceManager, "bloomPrefilter");

	//downsample pass
	//r.read("postProcess");
	//fs.currentRead().setAsTexture();
	//r.draw("bloom1");
	//r.fullScreenShader(e, "bloomDownsample");

	r.read("bloom1");
	fs.currentRead().setAsTexture();
	r.draw("bloom2");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom2");
	fs.currentRead().setAsTexture();
	r.draw("bloom3");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom3");
	fs.currentRead().setAsTexture();
	r.draw("bloom4");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom4");
	fs.currentRead().setAsTexture();
	r.draw("bloom5");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom5");
	fs.currentRead().setAsTexture();
	r.draw("bloom6");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom6");
	fs.currentRead().setAsTexture();
	r.draw("bloom7");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.read("bloom7");
	fs.currentRead().setAsTexture();
	r.draw("bloom8");
	r.fullScreenShader(resourceManager, "bloomDownsample");

	r.blendModeAdditive();

	r.read("bloom8");
	fs.currentRead().setAsTexture();
	r.draw("bloom7");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom7");
	fs.currentRead().setAsTexture();
	r.draw("bloom6");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom6");
	fs.currentRead().setAsTexture();
	r.draw("bloom5");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom5");
	fs.currentRead().setAsTexture();
	r.draw("bloom4");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom4");
	fs.currentRead().setAsTexture();
	r.draw("bloom3");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom3");
	fs.currentRead().setAsTexture();
	r.draw("bloom2");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom2");
	fs.currentRead().setAsTexture();
	r.draw("bloom1");
	r.fullScreenShader(resourceManager, "bloomUpsample");

	r.read("bloom1");
	fs.currentRead().setAsTexture();
	r.draw("main");
	r.fullScreenShader(resourceManager, "lastBloomUpsample");

	//r.read("postProcess");
	//fs.currentRead().setAsTexture();
	//r.draw("main");
	//r.fullScreenShader(e, "lastBloomUpsample");

	r.setAlphaBlending(false);
}
void TripleNineRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) {
		if (!window.capturing)window.captureCursor();
		else {
			if (window.renderer.idFramebuffer.objectID != -1) sim->enemies[window.renderer.idFramebuffer.objectID].die();
		}
	}
	if (input == exit) window.uncaptureCursor();

	if (input == reloadShaders) {
		window.renderer.shaderSystem.rebuild();
	}

	if (input == toggleBloom)bloom = !bloom;

	player.inputEvent(window, input);
}
void TripleNineRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	//track performance using optick
	OPTICK_EVENT();
	
	//set up variables for convenience
	Renderer& r = window.renderer;
	FramebufferSystem& fs = r.framebufferSystem;

	//default everything
	r.draw("main");
	r.clearDepth();
	r.uniforms().reset();
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(false);

	//render sky
	player.camera.renderOnlyRot(r);
	r.uniforms().sunDir(Vec4(sunDirection, 1));
	r.useTexture(resourceManager, "tripleNineSky");
	r.useShader(resourceManager, "tripleNineSky");
	r.renderMesh(resourceManager, "tripleNineSky");
	r.setDepthTest(true);
	r.setCulling(true);


	//render scene
	player.render(resourceManager, window);
	
	r.fill(Vec3(1));
	r.useShader(resourceManager, "tripleNineMap");
	r.useTexture(resourceManager, "tripleNineReflection");
	r.uniforms().mMatrix(matrixFromScale(mapSize));

	r.renderMesh(resourceManager, "tripleNineMap1");
	r.renderMesh(resourceManager, "tripleNineMap2");
	r.renderMesh(resourceManager, "tripleNineMap3");
	r.uniforms().mMatrix(Matrix(1));

	//global illumination test
	//r.useShader(e, "tripleNineFirstBounce");
	//r.renderMesh(e, "giTest");

	//render targets
	r.fill(Vec3(1,0.2,0));
	for (TripleNineEnemy& enemy : sim->enemies) enemy.render(resourceManager, r);

	//render sphere
	r.useShader(resourceManager, "color");
	r.uniforms().customColor(Vec4(1000000));
	r.uniforms().mMatrix(matrixFromLocation(Vec3(160, 50, 15 * pow(abs(sin(r.time.total*2)),0.5) + 1)));
	r.renderMesh(resourceManager, "sphere");

	//r.useShader(e, "normals");
	//r.renderMesh(e, "cube");


	r.setDepthTest(false);
	/*r.framebufferSystem.currentDraw().frameBufferTextures[2].texture.use(0);*/
	if (bloom) {
		renderBloom(resourceManager, r);
	}

	r.postProcess(resourceManager, "tonemapping");

	//ui
	//crosshair
	r.uniforms().reset();
	r.screenSpace();
	r.fill(0.1, 0.1, 0.1);
	r.useShader(resourceManager, "color");
	r.uniforms().mMatrix(matrixFromLocationAndSize(Vec2(window.getFrameSize() / 2), 3));
	r.renderMesh(resourceManager, "circle");
	r.fill(1, 1, 1);
	r.uniforms().mMatrix(matrixFromLocationAndSize(Vec2(window.getFrameSize() / 2), 2));
	r.renderMesh(resourceManager, "circle");

	r.screenSpace();
	Int i = 1;
	Float spacing = 30;
	r.uniforms().mMatrix(Matrix(1));
	r.textRenderSystem.setStyle(fonts.paragraph);
	r.textRenderSystem.alignText(Alignment::right, Alignment::top);
	r.textRenderSystem.render(resourceManager,r, "on Ground: " + toString(player.onGround), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "is moving: " + toString(player.isMoving), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "full body lean: " + toString(player.fullBodyLeanAngle), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "target id: " + toString(r.idFramebuffer.objectID), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "vertical speed: " + toString(player.velocity.z), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "double jump: " + toString(player.doubleJumpCharge), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "max jump height: " + toString(player.debugCurrentMaxJumpHeight), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "fps: " + toString(1 / r.time.delta), Vec2(spacing, i++ * spacing));
	r.textRenderSystem.render(resourceManager,r, "cameraForwardVector: " + toString(player.camera.forwardVector), Vec2(spacing, i++ * spacing));
}
void TripleNineRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	player.render(resourceManager, window);
	r.useShader(resourceManager, "idShader");

	r.setDepthTest(true);
	r.uniforms().mMatrix(matrixFromScale(mapSize));
	r.renderMesh(resourceManager, "tripleNineMap");
	r.uniforms().mMatrix(Matrix(1));

	for (TripleNineEnemy& enemy : sim->enemies) {
		enemy.renderInteractive(resourceManager, window, area);
	}
	r.setDepthTest(false);
}

void TripleNineEnemy::render(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.uniforms().mMatrix(matrixFromLocationAndSize(position, 1));
	renderer.renderMesh(resourceManager, "tripleNineTarget");
}
void TripleNineEnemy::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(position, 1));
	window.renderer.uniforms().objectID(id);
	window.renderer.renderMesh(resourceManager, "tripleNineTarget");
}

void TripleNine::run() {
	simulation.create();
	renderer.connect(simulation);
	ui.window("Triple Nine", Area(1920, 1080), true, WindowState::windowed, renderer, resourceManager);
	ui.run();
}