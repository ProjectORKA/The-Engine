#pragma once
#include "ASIONetworking.hpp"
#include "Game.hpp"
#include "ORKAIntroSequence.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "Networking.hpp"

#ifdef TRIPLE_NINE_ENEMY
struct TripleNineEnemy
{
	void update(Window& window);
	void create(Window& window);
	void destroy(Window& window);
	void setPosition(Vec3 position);
	void inputEvent(Window& window, InputEvent input);

private:
	Index enemyID = 0;
	Vec3 position = Vec3(0);
};
#endif

struct TripleNinePlayer final : Player
{
	// [TODO]
	// implement direction vectors separate from camera
	// crouching
	// proning
	// coyote time
	// sprint jump charge
	// either c alt or a and d to enable free cam while sprinting
	// pressed input
	InputId forward   = InputId(InputType::KeyBoard, W);
	InputId backward  = InputId(InputType::KeyBoard, S);
	InputId right     = InputId(InputType::KeyBoard, D);
	InputId left      = InputId(InputType::KeyBoard, A);
	InputId leanRight = InputId(InputType::KeyBoard, E);
	InputId leanLeft  = InputId(InputType::KeyBoard, Q);
	InputId run       = InputId(InputType::KeyBoard, Shift);
	InputId shoot     = InputId(InputType::Mouse, Lmb);
	InputId aim       = InputId(InputType::Mouse, Rmb);
	InputId holdJump  = InputId(InputType::KeyBoard, Space);
	InputId crouch    = InputId(InputType::KeyBoard, Ctrl);
	InputId precision = InputId(InputType::KeyBoard, Alt);
	// event input
	InputEvent jumpTrigger = InputEvent(InputType::KeyBoard, Space, true);

	// InputEvent jumpRelease = InputEvent(InputType::KeyBoard, SPACE, 0);
	enum class State : Byte
	{
		Standing,
		// ground
		Walking,
		// ground, slow move
		Sprinting,
		// ground, fast move
		Crouching,
		// ground, crouch
		Sneaking,
		// ground, crouch, slow move
		Sliding,
		// ground, crouch, fast move
		Proning,
		// ground, holding jump and crouch
		Crawling,
		// ground, holding jump and crouch, slow move
		Jumping,
		// air, positive z velocity
		Falling,
		// air, negative z velocity
	};

	// global scale
	const Float unit = 1.0f;
	// temporary input data
	Vec2  targetCameraRotation = Vec2(0.0f);
	Float inputTurnX           = 0.0f;
	Bool  movementInput        = false;
	State state                = State::Standing;
	// player vectors
	Vec3 forwardVector = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 rightVector   = Vec3(1.0f, 0.0f, 0.0f);
	// jump
	const Float minJumpVelocity = 10.0f * unit;
	const Float maxJumpVelocity = 15.0f * unit;
	Float       jumpCharge      = 0.0f;
	// Bool queueJump = false;
	Bool doubleJumpCharge = false;
	// speed
	const Float walkingSpeed             = 2.0f * unit;
	const Float runningSpeed             = 8.0f * unit;
	const Float speedControlAcceleration = 200.0f;
	const Float speedControlDeceleration = 1.0f;
	Bool        isMoving                 = false;
	Float       speedControl             = 0.0f;
	Vec3        movementControl          = Vec3(0.0f);
	// strafe
	const Float maxStrafeSpeed        = 5.0f;
	const Float maxStrafeAcceleration = 55.0f;
	// Vec2 strafeControl = Vec2(0);
	// player physique
	const Float height                     = 1.8f * unit; // total height when standing
	const Float lowerChestHeightMultiplier = 0.6f;        // height of lower chest based on total height //0.73
	const Float eyeHeightMultilpier        = 0.94f;       // height of camera based on total height
	const Float lowerChestHeight           = height * lowerChestHeightMultiplier;
	// head movement
	Float walkCycle  = 0;
	Vec3  headOffset = Vec3(0);
	// height
	const Float eyeHeightNormal    = height * eyeHeightMultilpier;         // standing
	const Float eyeHeightFlying    = height * 1.1f * eyeHeightMultilpier;  // off ground
	const Float eyeHeightWalking   = height * 0.9f * eyeHeightMultilpier;  // walking
	const Float eyeHeightCrouching = height * 0.5f * eyeHeightMultilpier;  // charging jump
	const Float eyeHeightRunning   = height * 0.75f * eyeHeightMultilpier; // running
	const Float eyeHeightProning   = height * 0.2f * eyeHeightMultilpier;  // proning
	const Float eyeHeightSliding   = height * 0.3f * eyeHeightMultilpier;  // sliding
	const Float eyeMovementSpeed   = 10.0f;
	Float       eyeHeight          = 0.0f;
	// headbob
	const Float headBobSpeed     = 1.4f;
	const Float headBobIntensity = 0.01f;
	Float       headBob          = 0.0f;
	// lean
	// upper body lean
	const Float upperBodyLeanSpeed          = 10.0f;
	const Float upperBodyMaxLeanAngle       = 0.8f;
	const Float upperBodyLeanHeadTiltFactor = 0.4f;
	Float       upperBodyLean               = 0.0f;
	Float       upperBodyLeanAngle          = 0.0f;
	Float       upperBodyLeanControl        = 0.0f;
	// full body lean
	const Float fullBodyLeanSpeed    = 5.0f;
	const Float fullBodyMaxLeanAngle = 0.6f;
	Float       fullBodyLean         = 0.0f;
	Float       fullBodyLeanAngle    = 0.0f;
	Float       fullBodyLeanControl  = 0.0f;
	// sway
	const Float headSwayImpact = 0.02f;
	Float       sway           = 0.0f;
	// state
	Bool  onGround         = false;
	Float actualSpeed      = 0.0f;
	Float targetSpeed      = walkingSpeed;
	Float actualInputSpeed = 0.0f;
	// physics
	Float       mass           = 80.0f; // kg
	Vec3        velocity       = Vec3(0.0f);
	Vec3        position       = Vec3(0.0f);
	Vec3        acceleration   = Vec3(0.0f);
	const Float gravity        = 30.0f; // gravity of player is more than 10 because it makes it feel snappier
	const Float airTimeGravity = 16.0f; // makes gravity less impactful when holding space in the air
	const Float airResistance  = 0.99f;
	// friction
	const Float iceFrictionFactor    = 0.1f;
	const Float normalFrictionFactor = 1.0f;
	Float       stopFriction         = 12.0f;
	Float       movementFriction     = 0.0f;
	Float       actualFriction       = stopFriction;
	// debug
	Float debugCurrentMaxJumpHeight = 0.0f;
	// aiming
	const Float  fovDefault              = 100.0f;
	const Float  fovScoped               = 45.0f;
	Float        fov                     = fovDefault;
	const Double mouseSensitivityDefault = 0.0015;
	const Double mouseSensitivityScoped  = 0.00075;
	const float  scopeSpeed              = 30.0f;

	[[nodiscard]] Bool isCollidingWithGround() const;

	void jump();
	void collisionResponse();
	void update(Window& window) override;
	void calculatePhysics(const Window& window);
	void calculateHeadPosition(Window& window, Float delta);
	void inputEvent(Window& window, InputEvent input) override;
};

struct TripleNinePlayerProxy
{
	Token playerToken = 0;
	Vec3  position    = Vec3(0.0f);
};

struct TripleNineGameState
{
	[[nodiscard]] ULL  getTick() const;
	[[nodiscard]] Vec3 getPlayerPosition(Token playerToken) const;

	void               updatePlayer(Token playerToken, Vec3 position);

	#ifdef TRIPLE_NINE_ENEMY
	[[nodiscard]] TripleNineEnemy& createEnemy();
	[[nodiscard]] const Vector<TripleNineEnemy>& getEnemies();
	#endif

private:
	#ifdef TRIPLE_NINE_ENEMY
	Vector<TripleNineEnemy>       enemies;
	#endif
	ULL                           gameTick = 0;
	Vector<TripleNinePlayerProxy> players;
};

enum class TripleNineClientToServerMessages : Byte
{
	WantsToConnect,
	SendPlayerData,
	WantsToDisconnect,
	IsAlive
};

enum class TripleNineServerToClientMessages : Byte
{
	DeclineConnection,
	ConfirmConnection, //send back id
	BroadcastPlayerData,
	BroadcastDisconnectClient,
	CheckIsAlive
};

struct TripleNineMessage
{
	union TripleNineEventID
	{
		TripleNineClientToServerMessages clientEvent = TripleNineClientToServerMessages::WantsToConnect;
		TripleNineServerToClientMessages serverEvent;
	};

	TripleNineEventID event;
	ULL               tick        = 0;
	Token             clientToken = 0;
	TimePoint         timeStamp   = now();
	Vec3              position    = Vec3(0);
	Vec3              rotation    = Vec3(0);
};

struct TripleNinePlayerData
{
	Vec3 position = Vec3(0);
	Vec3 rotation = Vec3(0);
};

struct TripleNineClientInfo
{
	UDPEndpoint          endpoint;
	TripleNinePlayerData playerData;
	Token                clientToken  = 0;
	TimePoint            lastSent     = now();
	TimePoint            lastReceived = now();
};

struct TripleNineServer
{
	void destroy();
	void create() const;
	void processEvents();

	void sendMessage();
	void broadcastMessage();

	// request
	void clientIsAlive();
	void disconnectClient();
	void clientWantsToConnect();

	// response
	void disconnectClients();
	void broadcastPlayerData();

protected:
	NetworkingContext            context;
	Vector<TripleNineClientInfo> clients;
	TripleNineMessage            inMessage;
	TripleNineMessage            outMessage;
	UDPEndpoint                  senderEndpoint;

	UShort    port              = 42069;
	TimePoint lastPingSent      = now();
	Duration  pingInterval      = Milliseconds(1000);
	Duration  deadClientTimeout = Milliseconds(15000);
	UDPSocket socket            = UDPSocket(context, UDPEndpoint(asio::ip::udp::v4(), port));
};

struct TripleNineSimulation final : GameSimulation
{
	TripleNineServer server;

	#ifdef TRIPLE_NINE_ENEMY
	void createEnemy();
	void killEnemy(const Index enemyId)
	{
		for (auto& enemy : gameState.getEnemies())
		{
			if (enemy.id == enemyId)
			{
				enemy.setPosition(Vec3(randomVec2Fast(-85.0f, 85.0f), 0.0f));
				break;
			}
		}
	}
	#endif

	void create() override;
	void destroy() override;
	void update(Float delta) override;
	void updatePlayer(Byte playerId, Vec3 position);

private:
	TripleNineGameState gameState;
};

struct TripleNineClient
{
	void create();
	void destroy();
	void sendMessage();
	void connectToServer();
	void disconnectFromServer();
	void waitTillConnected() const;

	//events
	void respondToPing();
	void connectionDeclined();
	void playerDataReceived();
	void playerDisconnected();
	void connectionConfirmed();

	[[nodiscard]] Token getClientToken() const;
	void                processEvents();

	void sendPlayerData(const Vec3 position, const Vec3 rotation, const ULL gameTick);

protected:
	NetworkingContext            context;
	TripleNineMessage            inMessage;
	TripleNineMessage            outMessage;
	UDPEndpoint                  serverEndpoint;
	Vector<TripleNinePlayerData> playerData;
	Vector<TripleNineClientInfo> clients;

	Token     myToken           = 0;
	UShort    port              = 42069;
	TimePoint lastSent          = now();
	TimePoint lastReceived      = now();
	Bool      connected         = false;
	String    host              = "localhost";
	Duration  keepAliveInterval = Milliseconds(10000);
	UDPSocket socket            = UDPSocket(context);
};

struct TripleNineRenderer final : GameRenderer
{
	TripleNineGameState gameState;

	TripleNineClient client;

	Framebuffer mainFramebuffer;
	Framebuffer bloom1Framebuffer;
	Framebuffer bloom2Framebuffer;
	Framebuffer bloom3Framebuffer;
	Framebuffer bloom4Framebuffer;
	Framebuffer bloom5Framebuffer;
	Framebuffer bloom6Framebuffer;
	Framebuffer bloom7Framebuffer;
	Framebuffer bloom8Framebuffer;

	TripleNinePlayer player;
	Float            mapSize    = 0.5f;
	Bool             vsync      = false;
	Bool             renderMap  = true;
	Bool             renderText = true;
	Bool             bloom      = false;

	Area       frameSize     = Area(1, 1);
	InputEvent enter         = InputEvent(InputType::Mouse, Lmb, true);
	InputEvent toggleBloom   = InputEvent(InputType::KeyBoard, B, false);
	InputEvent reloadShaders = InputEvent(InputType::KeyBoard, T, false);
	Vec3       sunDirection  = normalize(Vec3(0.675394f, -0.485956f, 0.554698f));

	void renderBloom(Renderer& r) const;
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;

	#ifdef TRIPLE_NINE_ENEMY
	void killEnemy(UInt uiId)
	{
		for (auto& enemy : gameState.enemies)
		{
			if (enemy.id == uiId)
			{
				enemy.position = Vec3(randomVec2Fast(-85.0f, 85.0f), 0.0f);
				break;
			}
		}
	}
	#endif

	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct TripleNine
{
	Bool                 useIntro = false;
	ORKAIntroSequence    intro;
	TripleNineRenderer   renderer1;
	TripleNineRenderer   renderer2;
	TripleNineSimulation simulation;

	void run();
};
