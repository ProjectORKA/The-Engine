#include "QuakeMovement.hpp"
#include "Window.hpp"
#include "Debug.hpp"



Index enemyID = 0;
void QuakePlayer::jump() {
	if (isMoving) {
		velocity.z = 0;
		velocity += Vec3(0, 0, minJumpVelocity);
	}
	else {
		velocity.z = 0;
		velocity += Vec3(0, 0, lerp(minJumpVelocity, maxJumpVelocity, jumpCharge));
		jumpCharge = 0;
	}
	queueJump = false;
	onGround = false;
	state = State::jumping;
}
void QuakePlayer::collisionResponse() {
	if (velocity.z < 0) velocity.z = 0;
	if (location.z < 0) location.z = 0;
}
Bool QuakePlayer::isCollidingWithGround() {
	return location.z <= 0;
}
void QuakePlayer::update(Window& window) {
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

	//jumping
	//when player stands still he can charge a high jump
	//if (window.pressed(holdJump) && onGround && velocity == Vec3(0))jumpCharge += delta;
	//else jumpCharge = 0;


	calculatePhysics(window);

	if (isCollidingWithGround()) {
		onGround = true;

		if (queueJump) {
			queueJump = false;
			jump();
		}

		collisionResponse();
		if (movementInput) actualFriction = movementFriction;
		else actualFriction = stopFriction;
		actualFriction *= normalFrictionFactor;
		velocity /= 1 + delta * actualFriction;

	}
	else onGround = false;

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
void QuakePlayer::calculatePhysics(Window& window) {
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
void QuakePlayer::inputEvent(Window& window, InputEvent input) {
	if (input == jumpTrigger) {
		if (onGround) {
			jump();
		} else {
			queueJump = true;
		}
	}
}
void QuakePlayer::calculateHeadPosition(Window& window, Float delta) {

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

void QuakeMovementRenderer::createEnemy() {
	enemies.emplaceBack();
	enemies.last().id = enemies.size() - 1;
}
void QuakeMovementRenderer::create(Engine& engine, Window& window) {
	player.camera.nearClipValue = 0.01;
	player.camera.farClipValue = 1000.0;

	for (Int i = 0; i < 100; i++) {
		createEnemy();
	}
}

void QuakeMovementRenderer::update(Window& window) {
	player.update(window);
}
void QuakeMovementRenderer::render(Engine& e, Window& window, TiledRectangle area) {
	OPTICK_EVENT();
	Renderer& r = window.renderer;

	r.setAlphaBlending(false);
	r.setCulling(true);
	r.setDepthTest(true);

	r.draw("main");
	r.clearColor(Color(0, 0, 0, 0));
	r.clearDepth();

	//r.blendModeAdditive();
	r.renderAtmosphere(e, player, sunDirection);
	//r.setAlphaBlending(false);

	r.uniforms().reset();

	player.render(e, window);

	r.useShader(e, "unlit");
	//r.useTexture(e, "quakeLightmapGround");
	//r.renderMesh(e, "quakeMapGround");

	//r.useTexture(e, "quakeLightmapSide1");
	//r.renderMesh(e, "quakeMapSide1");
	//r.useTexture(e, "quakeLightmapSide2");
	//r.renderMesh(e, "quakeMapSide2");
	//r.useTexture(e, "quakeLightmapSide3");
	//r.renderMesh(e, "quakeMapSide3");
	//r.useTexture(e, "quakeLightmapSide4");
	//r.renderMesh(e, "quakeMapSide4");
	r.uniforms().mMatrix(matrixFromScale(mapSize));
	r.useTexture(e, "moonolith");
	r.renderMesh(e, "moonolith");
	r.uniforms().mMatrix(Matrix(1));


	r.useTexture(e, "quakeMovementEnemy");
	for (QuakeEnemy& enemy : enemies) enemy.render(e, r);

	//r.arrow(e,player.location, player.location + player.velocity);

	//ui
	r.screenSpace();
	r.useShader(e, "color");
	r.fill(1, 1, 1);
	r.uniforms().mMatrix(matrixFromLocationAndSize(Vec2(window.getFrameSize() / 2), 2));
	r.renderMesh(e, "circle");
	r.fill(0.1, 0.1, 0.1);
	r.uniforms().mMatrix(matrixFromLocationAndSize(Vec2(window.getFrameSize() / 2), 3));
	r.renderMesh(e, "circle");

	r.screenSpace();
	Int i = 1;
	Float spacing = 30;
	r.renderText(e, "on Ground: " + toString(player.onGround), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "is moving: " + toString(player.isMoving), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "full body lean: " + toString(player.fullBodyLeanAngle), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "target id: " + toString(r.idFramebuffer.objectID), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "vertical speed: " + toString(player.velocity.z), Vec2(spacing, i++ * spacing), fonts.paragraph);
	r.renderText(e, "queue up jump: " + toString(player.queueJump), Vec2(spacing, i++ * spacing), fonts.paragraph);
}
void QuakeMovementRenderer::renderInteractive(Engine& e, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	player.render(e, window);
	r.useShader(e, "idShader");

	r.setDepthTest(true);
	r.uniforms().mMatrix(matrixFromScale(mapSize));
	r.renderMesh(e, "moonolith");
	r.uniforms().mMatrix(Matrix(1));

	for (QuakeEnemy& enemy : enemies) {
		enemy.renderInteractive(e, window, area);
	}
	r.setDepthTest(false);
}
void QuakeMovementRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) {
		if (!window.capturing)window.captureCursor();
		else {
			if (window.renderer.idFramebuffer.objectID != -1) enemies[window.renderer.idFramebuffer.objectID].die();
		}
	}
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}

void QuakeEnemy::render(Engine& engine, Renderer& renderer) {
	renderer.uniforms().mMatrix(matrixFromLocationAndSize(position, 0.5));
	renderer.renderMesh(engine, "quakeMovementEnemy");
}
void QuakeEnemy::renderInteractive(Engine& engine, Window& window, TiledRectangle area) {
	window.renderer.uniforms().mMatrix(matrixFromLocationAndSize(position, 0.5));
	window.renderer.uniforms().objectID(id);
	window.renderer.renderMesh(engine, "quakeMovementEnemy");
}


QuakeMovement::QuakeMovement(Engine& engine) {
	Window& w = window("ORKA Sandbox", Area(1920, 1080), true, WindowState::fullscreen, quakeMovementRenderer, engine);
	ui.run();
}