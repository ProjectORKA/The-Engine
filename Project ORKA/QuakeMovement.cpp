#include "QuakeMovement.hpp"
#include "Window.hpp"
#include "Debug.hpp"

void QuakePlayer::releaseJump() {
	if (onGround) {
		if (isMoving) {
			velocity += Vec3(0, 0, lerp(minJumpVelocity, maxJumpVelocity, speedControl));
		}
		else {
			velocity += Vec3(0, 0, lerp(minJumpVelocity, maxJumpVelocity, clamp(jumpCharge, 0, 1.1)));
			jumpCharge = 0;
		}
		onGround = false;
	}
}
void QuakePlayer::collisionResponse() {
	location.z = 0;
	velocity.z = 0;
}
void QuakePlayer::update(Window& window) {
	//set up temporary data
	Renderer& renderer = window.renderer;
	Float delta = renderer.deltaTime();

	//calculate player vectors
	forwardVector = normalize(Vec3(sin(camera.getRotationZ()), cos(camera.getRotationZ()), 0));
	rightVector = Vec3(forwardVector.y, -forwardVector.x, 0);

	//process input
	if(window.capturing)targetCameraRotation += window.mouseDelta * MouseMovement(mouseSensitivity);

	movementControl = Vec3(0);
	if (window.pressed(forward)) movementControl += forwardVector;
	if (window.pressed(backward)) movementControl -= forwardVector;
	if (window.pressed(right)) movementControl += rightVector;
	if (window.pressed(left)) movementControl -= rightVector;

	upperBodyLeanControl = 0;
	if (window.pressed(leanRight) && onGround) upperBodyLeanControl += 1;
	if (window.pressed(leanLeft) && onGround) upperBodyLeanControl -= 1;

	movementInput = movementControl != Vec3(0);

	//update values
		//running
	if (window.pressed(run)) speedControl += delta * speedControlAcceleration;
	else speedControl -= delta * speedControlDeceleration;

	speedControl = clamp(speedControl, 0, 1);

	camera.rotate(targetCameraRotation);

	//jumping
	if (window.pressed(chargeJump) && onGround)jumpCharge += delta;					//takes one second to charge to full jump height
	else jumpCharge = 0;

	calculatePhysics(delta);

	if (location.z <= 0) {
		onGround = true;
		collisionResponse();

		if (window.pressed(chargeJump)) releaseJump();

		if (onGround) {
			if (movementInput) actualFriction = movementFriction;
			else actualFriction = stopFriction;
			actualFriction *= normalFrictionFactor;
			velocity /= 1 + delta * actualFriction;
		}
	}
	else onGround = false;
	isMoving = velocity != Vec3(0);

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
void QuakePlayer::calculatePhysics(Float delta) {
	//step 1
	Vec3 velocity1 = velocity + acceleration * delta / Vec3(2);

	//step 2
	location += velocity1 * delta;

	//step 3 calculate forces

	//step4
	Vec3 velocity2 = velocity1 + acceleration * delta / Vec3(2);

	velocity = velocity2;
}
void QuakePlayer::inputEvent(Window& window, InputEvent input) {
	if (input == jumpRelease) releaseJump();
}
void QuakePlayer::calculateHeadPosition(Window& window, Float delta) {

	Float eyeHeightTarget = eyeHeightNormal;

	//advance walk cycle
	walkCycle += actualSpeed * delta / unit;

	//height
	if (onGround) {
		if (isMoving) {
			eyeHeightTarget = eyeHeightWalking;

			if (window.pressed(run)) eyeHeightTarget = eyeHeightRunning;
			else {
				if (window.pressed(chargeJump)) eyeHeightTarget = lerp(eyeHeightNormal, eyeHeightCrouching, clamp(jumpCharge, 0, 0.2));
				else eyeHeightTarget = eyeHeightNormal;
			}
		}
		else {
			if (window.pressed(chargeJump)) eyeHeightTarget = eyeHeightCrouching;
			else eyeHeightTarget = eyeHeightNormal;
		}
	}
	else eyeHeightTarget = eyeHeightFlying;


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

void QuakeMovementRenderer::update(Window& window) {
	player.update(window);
}
void QuakeMovementRenderer::render(Window& window, TiledRectangle area) {
	OPTICK_EVENT();
	Renderer& renderer = window.renderer;
	
	//renderer.renderAtmosphere(player, sunDirection);
	renderer.setCulling(true);
	renderer.setDepthTest(true);

	player.render(window);

	renderer.useShader("unlit");
	renderer.useTexture("quakeLightmapGround");
	renderer.renderMesh("quakeMapGround");

	renderer.useTexture("quakeLightmapSide1");
	renderer.renderMesh("quakeMapSide1");
	renderer.useTexture("quakeLightmapSide2");
	renderer.renderMesh("quakeMapSide2");
	renderer.useTexture("quakeLightmapSide3");
	renderer.renderMesh("quakeMapSide3");
	renderer.useTexture("quakeLightmapSide4");
	renderer.renderMesh("quakeMapSide4");

	//renderer.arrow(player.location, player.location + player.velocity);

	renderer.screenSpace();
	//renderer.renderText(toString(player.speedControl), Vec2(50), fonts.heading);
	//renderer.renderText(toString(player.onGround), Vec2(50, 100), fonts.heading);
	//renderer.renderText("horizontal speed: " + toString(length(player.velocity * Vec3(1, 1, 0))), Vec2(50, 150), fonts.heading);
	//renderer.renderText("is moving: " + toString(player.isMoving), Vec2(50, 200), fonts.heading);
	//renderer.renderText("full body lean: " + toString(player.fullBodyLeanAngle), Vec2(50, 250), fonts.heading);
	//renderer.renderText("upper body lean: " + toString(player.upperBodyLeanAngle), Vec2(50, 300), fonts.heading);
}
void QuakeMovementRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}