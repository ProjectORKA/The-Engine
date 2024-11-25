#include "Window.hpp"
#include "TripleNinePlayer.hpp"

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
