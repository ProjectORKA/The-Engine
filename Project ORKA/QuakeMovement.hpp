#pragma once

#include "Game.hpp"
#include "Player.hpp"

//required
//	physics

enum class PlayerState {
	standing,
	running,
	crouching,
	sliding,
	flying,
};

struct QuakePlayer : public Player {
	// [TODO]
	// queue up jump
	// implement direction vectors separate from camera
	// crouching
	// proning
	// coyote time
	// sprint jump charge
	// either c alt or a and d to enable free cam while sprinting
	
	//pressed input
	InputID forward = InputID(InputType::KeyBoard, W);
	InputID backward = InputID(InputType::KeyBoard, S);
	InputID right = InputID(InputType::KeyBoard, D);
	InputID left = InputID(InputType::KeyBoard, A);
	InputID leanRight = InputID(InputType::KeyBoard, E);
	InputID leanLeft = InputID(InputType::KeyBoard, Q);
	InputID run = InputID(InputType::KeyBoard, SHIFT);
	InputID shoot = InputID(InputType::Mouse, LMB);
	InputID aim = InputID(InputType::Mouse, RMB);
	InputID chargeJump = InputID(InputType::KeyBoard, SPACE);
	InputID crouch = InputID(InputType::KeyBoard, CTRL);
	InputID precision = InputID(InputType::KeyBoard, ALT);
	//event input
	InputEvent jumpRelease = InputEvent(InputType::KeyBoard, SPACE, 0);

	//temporary input data
	DVec2 targetCameraRotation = DVec2(0);
	Float inputTurnX = 0;
	Bool moving = false;

	const Float unit = 1;

	//jump
	const Float minJumpVelocity = 3 * unit;
	const Float maxJumpVelocity = 6 * unit;
	Float jumpCharge = 0;
	
	//speed
	const Float walkingSpeed = 2 * unit;	//
	const Float runningSpeed = 5 * unit;
	Float speedControl = 0;

	//strafe
	const Float maxStrafeSpeed = 1;
	const Float maxStrafeAcceleration = 1000000;

	//player physique
	const Float height = 1.8 * unit;			//total height when standing
	const Float lowerChestMultiplier = 0.73;	//height of camera based on total height
	const Float eyeHeightMultilpier = 0.94;		//height of lower chest based on total height


	//head movement
	Float walkCycle = 0;
	Vec3 headOffset = Vec3(0);
		//height

	const Float eyeHeightNormal = height * eyeHeightMultilpier;				//standing
	const Float eyeHeightFlying = height * 0.6 * eyeHeightMultilpier;		//off ground
	const Float eyeHeightWalking = height * 0.9 * eyeHeightMultilpier;		//walking
	const Float eyeHeightCrouching = height * 0.3 * eyeHeightMultilpier;	//charging jump
	const Float eyeHeightRunning = height * 0.55 * eyeHeightMultilpier;		//running

	const Float eyeMovementSpeed = 10;

	Float eyeHeight = 0;
		//bob
	const Float headBobSpeed = 3;
	const Float headBobIntensity = 0.02;
	Float headBob = 0;
		//lean
	const Float leanSpeed = 10;
	const Float leanAngle = 0.6;
	const Float leanAngleWhenMoving = 0.4;
	const Float runningLeanFactor = 4;
	Float leanTarget = 0.0;
	Float lean = 0;
		//sway
	const Float headSwayImpact = 0.02;
	Float sway = 0;
	
	//state
	Bool onGround = false;
	Float actualSpeed = 0;
	Float desiredSpeed = walkingSpeed;

	//physics
	Float mass = 80; //kg
	Vec3 velocity = Vec3(0);
	Vec3 location = Vec3(0, 0, 0);
	const Vec3 acceleration = Vec3(0, 0, -9.81);
	
	//friction
	const Float iceFrictionFactor = 0.1;
	const Float normalFrictionFactor = 1;
	Float stopFriction = 12;
	Float movementFriction = 6;
	Float actualFriction = stopFriction;

	void releaseJump();
	void collisionResponse();
	void calculatePhysics(Float delta);
	void calculateFriction(Float delta);
	void airStrafe(Window& window, Float delta);
	void calculateHeadPosition(Window & window, Float delta);

	void update(Window & window) override;
	void inputEvent(Window& window, InputEvent input) override;
	void mouseMoved(Window& window, MouseMovementInput input) override;
};

struct QuakeMovementRenderer : public GameRenderer {
	Vec3 sunDirection = normalize(Vec3(1, 0, 0.6));
	QuakePlayer player;
	Float playerSpeed = 1;

	InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
	InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);

	void update(Window& window) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void mouseMoved(Window& window, MouseMovementInput input) override;
};