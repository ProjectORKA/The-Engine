#pragma once
#include "Time.hpp"
#include "Basics.hpp"
#include "ASIONetworking.hpp"

enum class ClientToServerMessages : Byte
{
	WantsToConnect,
	SendPlayerData,
	WantsToDisconnect,
	IsAlive
};

enum class ServerToClientMessages : Byte
{
	DeclineConnection,
	ConfirmConnection, //send back id
	BroadcastPlayerData,
	BroadcastDisconnectClient,
	CheckIsAlive
};

struct PlayerData
{
	Vec3 position = Vec3(0);
	Vec3 rotation = Vec3(0);
};

union EventID
{
	ClientToServerMessages clientEvent;
	ServerToClientMessages serverEvent;
};

struct Message
{
	EventID event;
	Byte clientID = 0;
	TimePoint timeStamp;
	Vec3 position = Vec3(0);
	Vec3 rotation = Vec3(0);
};

struct ClientInfo
{
	UDPEndpoint endpoint;
	PlayerData  playerData;
	Short       clientID     = 0;
	TimePoint   lastSent     = now();
	TimePoint   lastReceived = now();
};

struct Server
{
	~Server();

	void        run();
	void        stop();
	void        start();
	void        sendMessage();
	void        broadcastMessage();
	void        removeDeadClients();
	void        waitTillReady() const;
	static void serverThread(Server& server);

	// request
	void clientIsAlive();
	void disconnectClient();
	void broadcastPlayerData();
	void clientWantsToConnect();

	// response
	void disconnectClients();
	void sendKeepAliveMessage();

private:
	Thread             thread;
	NetworkingContext  context;
	Vector<ClientInfo> clients;
	Message            inMessage;
	Message            outMessage;
	UDPEndpoint        senderEndpoint;

	UShort    runningNewClientId = 0;
	Bool      running            = true;
	Bool      keepRunning        = true;
	UShort    port               = 42069;
	Duration  pingInterval       = Milliseconds(1000);
	Duration  deadClientTimeout  = Milliseconds(5000);
	TimePoint lastPingSent       = now();
	UDPSocket socket             = UDPSocket(context, UDPEndpoint(asio::ip::udp::v4(), port));
};

struct Client
{
	Vector<PlayerData> playerData;

	~Client();

	void        run();
	void        stop();
	void        start();
	void        sendMessage();
	void        connectToServer();
	void        disconnectFromServer();
	void        waitTillConnected() const;
	static void clientThread(Client& client);

	//events
	void respondToPing();
	void connectionDeclined();
	void playerDataReceived();
	void playerDisconnected();
	void connectionConfirmed();

private:
	Thread            thread;
	NetworkingContext context;
	Message           inMessage;
	Message           outMessage;
	UDPEndpoint       serverEndpoint;

	Vector<ClientInfo> clients;

	Byte      clientID          = 0;
	Bool      keepRunning       = true;
	UShort    port              = 42069;
	TimePoint lastSent          = now();
	TimePoint lastReceived      = now();
	Bool      connected         = false;
	String    host              = "localhost";
	Duration  keepAliveInterval = Milliseconds(5000);
	UDPSocket socket            = UDPSocket(context);
};
