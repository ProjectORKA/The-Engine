#pragma once
#include "ASIONetworking.hpp"
#include "Basics.hpp"
#include "Time.hpp"

struct Message
{
	Byte eventID  = 0;
	Byte clientID = 0;
};

struct ClientInfo
{
	UDPEndpoint endpoint;
	Short       clientID     = 0;
	TimePoint   lastSent     = TimePoint::min();
	TimePoint   lastReceived = TimePoint::min();
};

struct Server
{
	void        run();
	void        stop();
	void        start();
	void        sendMessage();
	void        waitTillReady() const;
	static void serverThread(Server& server);
	void        disconnectClients();

	//events
	void clientIsAlive();
	void disconnectClient();
	void registerNewClient();

private:
	UShort             runningNewClientId = 0;
	Thread             thread;
	NetworkingContext  context;
	Vector<ClientInfo> clients;
	Message            inMessage;
	Message            outMessage;
	UDPEndpoint        senderEndpoint;

	Bool      running     = true;
	Bool      keepRunning = true;
	UShort    port        = 42069;
	UDPSocket socket      = UDPSocket(context, UDPEndpoint(asio::ip::udp::v4(), port));
};

struct Client
{
	void        run();
	void        stop();
	void        start();
	void        sendMessage();
	void        waitTillConnected() const;
	static void clientThread(Client& client);
	void        customEvents(Message& message);;
	//events
	void connectToServer();
	void disconnectFromServer();
	void changeClientID(UShort clientID);

private:
	Thread            thread;
	NetworkingContext context;
	Message           inMessage;
	Message           outMessage;
	UDPEndpoint       serverEndpoint;

	Byte      clientID    = 0;
	Bool      keepRunning = true;
	UShort    port        = 42069;
	Bool      connected   = false;
	String    host        = "localhost";
	UDPSocket socket      = UDPSocket(context);
};
