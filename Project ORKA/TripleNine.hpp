#pragma once

#include "Game.hpp"
#include "Player.hpp"
#include "Profiler.hpp"
#include "Random.hpp"

//required
//	physics
//	crouch walking
//	sliding
//  bullet traces
//  targets explode
//		decal splatter on objects

	enum class PlayerState {
		standing,
		running,
		crouching,
		sliding,
		flying,
	};

	extern Index enemyID;

	struct TripleNineEnemy : UIElement{
		Vec3 position = Vec3(randomVec2(-85, 85), 0);
		Index id = -1;

		void die() {
			position = Vec3(randomVec2(-85, 85), 0);
		}
		void render(Engine& engine, Renderer& renderer);
		void renderInteractive(Engine& engine, Window& window, TiledRectangle area) override;
	};

	struct TripleNinePlayer : public Player {
		// [TODO]
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
		InputID holdJump = InputID(InputType::KeyBoard, SPACE);
		InputID crouch = InputID(InputType::KeyBoard, CTRL);
		InputID precision = InputID(InputType::KeyBoard, ALT);
		//event input
		InputEvent jumpTrigger = InputEvent(InputType::KeyBoard, SPACE, 1);
		//InputEvent jumpRelease = InputEvent(InputType::KeyBoard, SPACE, 0);

		enum class State {
			standing,		// ground
			walking,		// ground, slowmove
			sprinting,		// ground, fastmove
			crouching,		// ground, crouch
			sneaking,		// ground, crouch, slowmove
			sliding,		// ground, crouch, fastmove
			proning,		// ground, holding jump and crouch
			crawling,		// ground, holding jump and crouch, slowmove
			jumping,		// air, positive z velocity
			falling,		// air, negative z velocity
		};

		//global scale
		const Float unit = 1;

		//temporary input data
		DVec2 targetCameraRotation = DVec2(0);
		Float inputTurnX = 0;
		Bool movementInput = false;

		State state = State::standing;

		//player vectors
		Vec3 forwardVector = Vec3(0, 1, 0);
		Vec3 rightVector = Vec3(1, 0, 0);

		//jump
		const Float minJumpVelocity = 10 * unit;
		const Float maxJumpVelocity = 15 * unit;
		Float jumpCharge = 0;
		//Bool queueJump = false;
		Bool doubleJumpCharge = false;

		//speed
		const Float walkingSpeed = 2 * unit;	//
		const Float runningSpeed = 8 * unit;
		const Float speedControlAcceleration = 200;
		const Float speedControlDeceleration = 1;
		Bool isMoving = false;
		Float speedControl = 0;
		Vec3 movementControl = Vec3(0);

		//strafe
		const Float maxStrafeSpeed = 5;
		const Float maxStrafeAcceleration = 55;
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
		const Float eyeHeightFlying = height * 1.1 * eyeHeightMultilpier;		//off ground
		const Float eyeHeightWalking = height * 0.9 * eyeHeightMultilpier;		//walking
		const Float eyeHeightCrouching = height * 0.5 * eyeHeightMultilpier;	//charging jump
		const Float eyeHeightRunning = height * 0.75 * eyeHeightMultilpier;		//running
		const Float eyeHeightProning = height * 0.2 * eyeHeightMultilpier;		//proning
		const Float eyeHeightSliding = height * 0.3 * eyeHeightMultilpier;		//sliding

		const Float eyeMovementSpeed = 10;

		Float eyeHeight = 0;

		//headbob
		const Float headBobSpeed = 1.4;
		const Float headBobIntensity = 0.01;
		Float headBob = 0;

		//lean
		//upper body lean
		const Float upperBodyLeanSpeed = 10;
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
		Vec3 acceleration = Vec3(0, 0, 0);
		const Float gravity = 30;	//gravity of player is more than 10 because it makes it feel snappier
		const Float airTimeGravity = 16; //makes gravity less impactful when holding space in the air
		const Float airResistance = 0.99;

		//friction
		const Float iceFrictionFactor = 0.1;
		const Float normalFrictionFactor = 1;
		Float stopFriction = 12;
		Float movementFriction = 0;
		Float actualFriction = stopFriction;

		//debug
		Float debugCurrentMaxJumpHeight = 0;

		void jump();
		void collisionResponse();
		Bool isCollidingWithGround();
		void update(Window& window) override;
		void calculatePhysics(Window& window);
		void calculateHeadPosition(Window& window, Float delta);
		void inputEvent(Window& window, InputEvent input) override;
	};

	struct TripleNineRenderer : public GameRenderer {
		Bool bloom = true;
		TripleNinePlayer player;
		Float mapSize = 0.85;
		Vec3 sunDirection = normalize(Vec3(1, 0, 0.6));

		Vector<TripleNineEnemy> enemies;

		InputEvent enter = InputEvent(InputType::Mouse, LMB, 1);
		InputEvent exit = InputEvent(InputType::Mouse, RMB, 0);
		InputEvent toggleBloom = InputEvent(InputType::KeyBoard, B, 0);
		InputEvent reloadShaders = InputEvent(InputType::KeyBoard, T, 0);

		void createEnemy();
		void update(Window& window) override;
		void renderBloom(Engine& e, Renderer& r);
		void create(Engine& engine, Window& window) override;
		void inputEvent(Window& window, InputEvent input) override;
		void render(Engine& engine, Window& window, TiledRectangle area) override;
		void renderInteractive(Engine& engine, Window& window, TiledRectangle area) override;;
	};


struct TripleNine {
	TripleNineRenderer gameRenderer;
	TripleNine(Engine& engine);
};