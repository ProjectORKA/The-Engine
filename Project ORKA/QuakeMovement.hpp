#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "Profiler.hpp"

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
	// fix speed
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

	//global scale
	const Float unit = 1;

	//temporary input data
	DVec2 targetCameraRotation = DVec2(0);
	Float inputTurnX = 0;
	Bool movementInput = false;

	//player vectors
	Vec3 forwardVector = Vec3(0, 1, 0);
	Vec3 rightVector = Vec3(1, 0, 0);

	//jump
	const Float minJumpVelocity = 3 * unit;
	const Float maxJumpVelocity = 5 * unit;
	Float jumpCharge = 0;
	
	//speed
	const Float walkingSpeed = 1 * unit;	//
	const Float runningSpeed = 5.5 * unit;
	const Float speedControlAcceleration = 100;
	const Float speedControlDeceleration = 1;
	Bool isMoving = false;
	Float speedControl = 0;
	Vec3 movementControl = Vec3(0);

	//strafe
	const Float maxStrafeSpeed = 3;
	const Float maxStrafeAcceleration = 100;
	//Vec2 strafeControl = Vec2(0);

	//player physique
	const Float height = 1.8 * unit;				//total height when standing
	const Float lowerChestHeightMultiplier = 0.6;	//height of lower chest based on total height //0.73
	const Float eyeHeightMultilpier = 0.94;			//height of camera based on total height
	const Float lowerChestHeight = height * lowerChestHeightMultiplier;

	//head movement
	Float walkCycle = 0;
	Vec3 headOffset = Vec3(0);
		//height

	const Float eyeHeightNormal = height * eyeHeightMultilpier;				//standing
	const Float eyeHeightFlying = height * 0.6 * eyeHeightMultilpier;		//off ground
	const Float eyeHeightWalking = height * 0.9 * eyeHeightMultilpier;		//walking
	const Float eyeHeightCrouching = height * 0.3 * eyeHeightMultilpier;	//charging jump
	const Float eyeHeightRunning = height * 0.75 * eyeHeightMultilpier;		//running

	const Float eyeMovementSpeed = 10;

	Float eyeHeight = 0;
	
	//headbob
	const Float headBobSpeed = 2.6;
	const Float headBobIntensity = 0.01;
	Float headBob = 0;
	
	//lean
	//upper body lean
	const Float upperBodyLeanSpeed = 20;
	const Float upperBodyMaxLeanAngle = 0.8;
	const Float upperBodyLeanHeadTiltFactor = 0.4;
	Float upperBodyLean = 0;
	Float upperBodyLeanAngle = 0;
	Float upperBodyLeanControl = 0.0;
	//full body lean
	const Float fullBodyLeanSpeed = 5;
	const Float fullBodyMaxLeanAngle = 0.6;
	Float fullBodyLean = 0;
	Float fullBodyLeanAngle = 0;
	Float fullBodyLeanControl = 0.0;

	//sway
	const Float headSwayImpact = 0.02;
	Float sway = 0;
	
	//state
	Bool onGround = false;
	Float actualSpeed = 0;
	Float targetSpeed = walkingSpeed;
	Float actualInputSpeed = 0;

	//physics
	Float mass = 80; //kg
	Vec3 velocity = Vec3(0);
	Vec3 location = Vec3(0, 0, 0);
	const Vec3 acceleration = Vec3(0, 0, -9.81);
	
	//friction
	const Float iceFrictionFactor = 0.1;
	const Float normalFrictionFactor = 1;
	Float stopFriction = 12;
	Float movementFriction = 0;
	Float actualFriction = stopFriction;

	void releaseJump();
	void collisionResponse();
	void calculatePhysics(Float delta);
	void calculateHeadPosition(Window & window, Float delta);

	void update(Window & window) override;
	void inputEvent(Window& window, InputEvent input) override;
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
};