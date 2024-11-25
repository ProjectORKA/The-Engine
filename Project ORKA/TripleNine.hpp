#pragma once
#include "ASIONetworking.hpp"
#include "Game.hpp"
#include "ORKAIntroSequence.hpp"
#include "Player.hpp"
#include "Random.hpp"
#include "TripleNineMessage.hpp"
#include "TripleNinePlayer.hpp"

// [TODO]
// calculate ping
// store ping in client info

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

struct TripleNinePlayerData
{
	PlayerID playerID = 0;
	Vec3     position = Vec3(0);
	Vec3     rotation = Vec3(0);
};

struct TripleNineGameState
{
	[[nodiscard]] ULL  getTick() const;
	[[nodiscard]] Vec3 getPlayerPosition(PlayerID playerID) const;

	void updatePlayer(PlayerID playerID, Vec3 position);

	#ifdef TRIPLE_NINE_ENEMY
	[[nodiscard]] TripleNineEnemy& createEnemy();
	[[nodiscard]] const Vector<TripleNineEnemy>& getEnemies();
	#endif

private:
	#ifdef TRIPLE_NINE_ENEMY
	Vector<TripleNineEnemy>       enemies;
	#endif
	ULL                          gameTick = 0;
	Vector<TripleNinePlayerData> players;
};

struct TripleNineClientInfo
{
	UDPEndpoint endpoint;
	Token       clientToken  = 0;
	ClientID    clientID     = 0;
	Float       ping         = 0.0;
	TimePoint   lastSent     = now();
	TimePoint   lastReceived = now();
};

struct TripleNineServer
{
	void destroy();
	void create() const;
	void processEvents();
	void sendBackMessage(TripleNineMessage& message);
	void broadcastMessage(TripleNineMessage& message);
	void sendMessageToClient(ClientID clientID, TripleNineMessage& message);

	// request
	void getServerInfo();
	void receivedPing(TimePoint timeStamp);
	void clientWantsToConnect();
	void disconnectClient(const DisconnectClientData& disconnectClientData);

	// response
	void disconnectAllClients();
	//void broadcastPlayerData();

protected:
	NetworkingContext            context;
	Vector<TripleNineClientInfo> clients;
	UDPEndpoint                  senderEndpoint;

	UShort    port              = 42069;
	TimePoint lastPingSent      = now();
	Duration  pingInterval      = Milliseconds(1000);
	Duration  deadClientTimeout = Milliseconds(15000);
	UDPSocket socket            = UDPSocket(context, UDPEndpoint(asio::ip::udp::v4(), port));
};

struct TripleNineClient
{
	void create();
	void destroy();
	void processEvents();
	void waitTillConnected() const;
	void sendMessage(TripleNineMessage& message);

	//receive
	void respondToPing();
	void connectionDeclined();
	void receiveServerInfo(const ServerInfoData& serverInfoData);
	void disconnect(const DisconnectClientData& disconnectClientData);
	void connectionConfirmed(const ConnectionConfirmedData& connectionConfirmedData);

	//send
	void connectToServer();
	void requestServerInfo();
	void disconnectFromServer();
	void disconnectPlayer(PlayerID playerID);

	//void sendPlayerData(PlayerID playerID, Token playerToken, Vec3 position, Vec3 rotation, ULL gameTick);
	//[[nodiscard]] Token getClientToken() const;
	//PlayerID getPlayerID() const;
	//void playerDataReceived(PlayerID playerID, const TripleNinePlayerData& playerData);

protected:
	NetworkingContext            context;
	UDPEndpoint                  serverEndpoint;
	Vector<TripleNinePlayerData> playerData;

	Token     authToken         = 0;
	ClientID  clientID          = 0;
	UShort    port              = 42069;
	TimePoint lastSent          = now();
	TimePoint lastReceived      = now();
	Bool      connected         = false;
	String    host              = "localhost";
	Duration  keepAliveInterval = Milliseconds(10000);
	UDPSocket socket            = UDPSocket(context);
};

struct TripleNineSimulation final : GameSimulation
{
	Bool             isOnline = false;
	Bool             isServer = true;
	TripleNineServer server;
	TripleNineClient client;

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
