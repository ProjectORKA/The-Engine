#include "QuakeMovement.hpp"
#include "Window.hpp"

void QuakePlayer::collisionResponse() {
	location.z = 0;
	velocity.z = 0;
}
void QuakePlayer::update(Window & window) {
	//set up temporary data
	Renderer& renderer = window.renderer;
	Vec3 movementVector = Vec3(0);
	Float delta = renderer.deltaTime();

	//process input
	if (window.pressed(forward)) movementVector += camera.forwardVector;
	if (window.pressed(backward)) movementVector -= camera.forwardVector;
	if (window.pressed(right)) movementVector += camera.rightVector;
	if (window.pressed(left)) movementVector -= camera.rightVector;

	leanTarget = 0;
	if (window.pressed(leanLeft) && onGround && !window.pressed(run)) leanTarget += 1;
	if (window.pressed(leanRight) && onGround && !window.pressed(run)) leanTarget -= 1;

	moving = length(movementVector) > 0;

	//update values
		//running
	if (window.pressed(run)) speedControl += delta;
	if (window.pressed(precision)) speedControl -= delta;
	speedControl = clamp(speedControl, 0, 1);
	
	desiredSpeed = lerp(walkingSpeed, runningSpeed, speedControl);

	camera.rotate(targetCameraRotation);

	//jumping
	if (window.pressed(chargeJump) && onGround)jumpCharge += delta;					//takes one second to charge to full jump height
	else jumpCharge = 0;
	
	calculatePhysics(delta);

	if (location.z <= 0) {
		onGround = true;
		collisionResponse();
		calculateFriction(delta);
	}
	else onGround = false;

	calculateHeadPosition(window,delta);

	if (moving && onGround) {
		//get just horizontal component of acceleration
		movementVector.z = 0;
		//get direction of movement (just direction)
		movementVector = normalize(movementVector);
		//add speed to direction
		movementVector *= desiredSpeed;

		velocity.x = movementVector.x;
		velocity.y = movementVector.y;

		movementVector = { 0, 0, 0 };

		moving = false;
	}

	//reset delta
	targetCameraRotation = DVec2(0);
}
void QuakePlayer::releaseJump(Window& window) {
	if (onGround) {
		if (moving) {
			if(window.pressed(run)){
				velocity += Vec3(0,0,maxJumpVelocity);
			}
			else {
				beep();
				velocity += Vec3(0, 0, minJumpVelocity);
			}
		}
		else {
			velocity += Vec3(0, 0, lerp(minJumpVelocity,maxJumpVelocity,clamp(jumpCharge,0, 1.1)));
			jumpCharge = 0;
		}
		onGround = false;
	}
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

	actualSpeed = length(velocity);
}
void QuakePlayer::calculateFriction(Float delta) {

	if (moving) actualFriction = movementFriction;
	else actualFriction = stopFriction;

	actualFriction *= normalFrictionFactor;

	velocity /= 1 + delta * actualFriction;
}
void QuakePlayer::inputEvent(Window& window, InputEvent input) {
	if (input == jumpRelease) releaseJump(window);
}
void QuakePlayer::calculateHeadPosition(Window & window, Float delta) {
	
	Float eyeHeightTarget = eyeHeightNormal;
	
	//advance walk cycle
	walkCycle += actualSpeed * delta / unit;

	//height
	if (onGround) {
		if (moving) {
			eyeHeightTarget = eyeHeightWalking;
			
			if (window.pressed(run)) eyeHeightTarget = eyeHeightRunning;
			else {
				if (window.pressed(chargeJump)) eyeHeightTarget = lerp(eyeHeightNormal, eyeHeightCrouching, clamp(jumpCharge,0,0.2));
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
	if(moving && onGround) bobTarget = sin(lerp(1, headBobSpeed, 0.5) * 2 * walkCycle) * desiredSpeed * headBobIntensity;

	//sway
	Float swayTarget = 0;
	if (onGround) swayTarget = sin(lerp(1, headBobSpeed, 0.5) *  walkCycle) * actualSpeed * headSwayImpact;
	sway = swayTarget;

	//lean
	if (onGround && moving) leanTarget += runningLeanFactor * desiredSpeed * -targetCameraRotation.x / unit;

	//smooth motaion
	approach(lean,leanTarget,delta * leanSpeed);
	approach(eyeHeight, eyeHeightTarget, delta * eyeMovementSpeed);

	//leanOffset based on lean angle
	Float lowerChestToEyeDistance = height * (1 - lowerChestMultiplier);
	Float leanOffsetX = lowerChestToEyeDistance * sin(lean * leanAngle);
	Float leanOffsetZ = lowerChestToEyeDistance * cos(lean * leanAngle) - lowerChestToEyeDistance;

	//apply changes to camera
	camera.setRotation(DVec3(camera.getRotation().x, lean * leanAngle, camera.getRotation().z));
	//calculate 3d position of all offsets
	Vec3 sway3D = -(lean + sway) * normalize(camera.rightVector * Vec3(1, 1, 0));
	Vec3 lean3D = leanOffsetX * camera.rightVector + Vec3(0, 0, leanOffsetZ);
	Vec3 headHeight3D = Vec3(0, 0, eyeHeight + bobTarget);

	camera.location = location + sway3D + headHeight3D + lean3D;
}
void QuakePlayer::mouseMoved(Window& window, MouseMovementInput input) {
	targetCameraRotation += input * DVec2(mouseSensitivity);
}

void QuakeMovementRenderer::update(Window& window) {
	player.update(window);
}
void QuakeMovementRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}
void QuakeMovementRenderer::mouseMoved(Window& window, MouseMovementInput input) {
	player.mouseMoved(window, input);
	
}
void QuakeMovementRenderer::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	renderer.renderAtmosphere(player, sunDiretion);

	player.render(window);

	renderer.useShader("unlit");
	renderer.useTexture("lightmap");
	renderer.renderMesh("quakeMap");

	renderer.screenSpace();
	renderer.renderText(toString(player.speedControl), Vec2(50), fonts.heading);
}