#include "Networking.hpp"
#include "Debug.hpp"
#include "ASIONetworking.hpp"

Server::~Server()
{
	stop();
}

void Server::run()
{
	logDebug("Server: Listening...");
	while (keepRunning)
	{
		running = true;
		Array<Char, sizeof(Message)> inMessageBuffer;
		if (socket.available())
		{
			socket.receive_from(asio::buffer(inMessageBuffer, sizeof(Message)), senderEndpoint);
			inMessage = *reinterpret_cast<Message*>(&inMessageBuffer);
			switch (inMessage.event.clientEvent)
			{
			case ClientToServerMessages::WantsToConnect: clientWantsToConnect();
				break;
			case ClientToServerMessages::SendPlayerData: broadcastPlayerData();
				break;
			case ClientToServerMessages::WantsToDisconnect: disconnectClient();
				break;
			case ClientToServerMessages::IsAlive: clientIsAlive();
				break;
			default: break;
			}
		}

		removeDeadClients();

		sendKeepAliveMessage();
	}

	disconnectClients();
}

void Server::sendMessage()
{
	outMessage.timeStamp = now();
	for (ClientInfo& client : clients)
	{
		if (client.clientID == outMessage.clientID)
		{
			client.lastSent                               = now();
			Array<Char, sizeof(Message)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(Message)>*>(&outMessage);
			socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
			return;
		}
	}
}

void Server::broadcastMessage()
{
	outMessage.timeStamp = now();
	for (ClientInfo& client : clients)
	{
		client.lastSent                               = now();
		Array<Char, sizeof(Message)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(Message)>*>(&outMessage);
		socket.send_to(asio::buffer(outMessageBuffer), client.endpoint);
	}
}

void Server::waitTillReady() const
{
	while (!running) {}
}

void Server::removeDeadClients()
{
	for (Int i = 0; i < clients.size(); i++)
	{
		if (now() - clients[i].lastReceived > deadClientTimeout)
		{
			logDebug("Server: Client (" + toString(clients[i].clientID) + ") timed out");
			clients.erase(clients.begin() + i);
			i--;
		}
	}
}

void Server::broadcastPlayerData()
{
	UShort currentClient = inMessage.clientID;

	for (ClientInfo& client : clients)
	{
		if (client.clientID == currentClient)
		{
			client.playerData.position = inMessage.position;
			client.playerData.rotation = inMessage.rotation;
			continue;
		}
		outMessage.event.serverEvent  = ServerToClientMessages::BroadcastPlayerData;
		outMessage.clientID = client.clientID;
		outMessage.position = inMessage.position;
		outMessage.rotation = inMessage.rotation;
		sendMessage();
	}
}

void Server::clientWantsToConnect()
{
	logDebug("Server: Registering new client...");

	if (inMessage.clientID != 0) logDebug("Server: Warning: Client already has id");

	Bool newIdAvailable;

	logDebug(senderEndpoint.address().to_string() + ":" + toString(senderEndpoint.port()));

	do
	{
		if (runningNewClientId == 0) runningNewClientId++;

		newIdAvailable = false;
		for (ClientInfo& client : clients)
		{
			if (client.endpoint == senderEndpoint)
			{
				//send him his already existing id
				outMessage.event.serverEvent  = ServerToClientMessages::ConfirmConnection;
				outMessage.clientID = client.clientID;
				logDebug("Server: Client (" + toString(outMessage.clientID) + ") already exists, sending again");
				sendMessage();
				return;
			}
			if (client.clientID == runningNewClientId)
			{
				newIdAvailable = true;
				logDebug("Server: ID already in use, trying next id");
			}
		}
		if (newIdAvailable) runningNewClientId++;
	}
	while (newIdAvailable);

	// store info
	ClientInfo info;
	info.clientID = runningNewClientId;
	info.endpoint = senderEndpoint;
	clients.push_back(info);

	// give client its new id
	outMessage.event.serverEvent  = ServerToClientMessages::ConfirmConnection;
	outMessage.clientID = runningNewClientId;
	logDebug("Server: New client registered");

	logDebug("Server: Client assigned id: " + toString(outMessage.clientID));
	sendMessage();
}

void Server::disconnectClient()
{
	logDebug("Server: Client (" + toString(inMessage.clientID) + ") disconnecting");
	for (Int i = 0; i < clients.size(); i++)
	{
		if (clients[i].clientID == inMessage.clientID)
		{
			clients.erase(clients.begin() + i);
		}
	}
}

void Server::serverThread(Server& server)
{
	try
	{
		server.run();
	}
	catch (Exception& e)
	{
		logError(e.what());
	}
}

void Server::disconnectClients()
{
	logDebug("Server: Disconnecting clients...");
	for (ClientInfo& client : clients)
	{
		logDebug("Server: Disconnecting client (" + toString(client.clientID) + ")");
		outMessage.clientID = client.clientID;
		outMessage.event.serverEvent  = ServerToClientMessages::BroadcastDisconnectClient;
		broadcastMessage();
	}
	clients.clear();
}

void Server::sendKeepAliveMessage()
{
	if (now() - lastPingSent > pingInterval)
	{
		lastPingSent = now();
		outMessage.clientID          = 0;
		outMessage.event.serverEvent = ServerToClientMessages::CheckIsAlive;
		broadcastMessage();
	}
}

void Server::clientIsAlive()
{
	//calculate ping

	for (ClientInfo& client : clients)
	{
		if (client.endpoint == senderEndpoint)
		{
			client.lastReceived = now();
		}
	}
}

void Server::start()
{
	logDebug("Server: Starting...");
	thread = Thread(serverThread, std::ref(*this));
}

void Server::stop()
{
	if (keepRunning)
	{
		logDebug("Server: Shutting down...");
		keepRunning = false;
		thread.join();
		logDebug("Server: Shut down complete");
	}
}

void Client::start()
{
	logDebug("Client: Starting...");
	thread = Thread(clientThread, std::ref(*this));
}

void Client::stop()
{
	if (keepRunning)
	{
		logDebug("Client (" + toString(clientID) + "): Shutting down...");
		keepRunning = false;
		thread.join();
		logDebug("Client (" + toString(clientID) + "): Shut down complete");
	}
}

Client::~Client()
{
	stop();
}

void Client::respondToPing()
{
	logDebug("Client (" + toString(clientID) + "): I am alive");
	outMessage.event.clientEvent = ClientToServerMessages::IsAlive;
	outMessage.clientID          = clientID;
	sendMessage();
}

void Client::connectionDeclined()
{
	logDebug("Client (" + toString(clientID) + "): Server refused connection");
	connected = false;
	keepRunning = false;
}

void Client::playerDataReceived()
{
	// update player data
	for (ClientInfo& client : clients)
	{
		if (client.clientID == inMessage.clientID)
		{
			client.playerData.position = inMessage.position;
			client.playerData.rotation = inMessage.rotation;
			return;
		}
	}

	// must be new client
	ClientInfo newClient;
	newClient.clientID            = inMessage.clientID;
	newClient.playerData.position = inMessage.position;
	newClient.playerData.rotation = inMessage.rotation;
	clients.push_back(newClient);
}

void Client::playerDisconnected()
{
	if (inMessage.clientID == clientID)
	{
		// disconnect
		keepRunning = false;
		connected   = false;
		logDebug("Client (" + toString(clientID) + "): Server requested disconnect!");
	}
	else
	{
		for (Int i = 0; i < clients.size(); i++)
		{
			// [TODO] remove player
			if (clients[i].clientID == inMessage.clientID)
			{
				clients.erase(clients.begin() + i);
				logDebug("Client (" + toString(clientID) + "): Player (" + toString(inMessage.clientID) + ") disconnected");
				return;
			}
		}
	}
}

void Client::connectionConfirmed()
{
	logDebug("Client (" + toString(clientID) + "): Server confirmed connection");
	logDebug("Client (" + toString(clientID) + "): Server requested ID change");
	this->clientID = inMessage.clientID;
	logDebug("Client (" + toString(clientID) + "): Client was assigned ID:" + toString(clientID));
	if (!connected)
	{
		logDebug("Client (" + toString(clientID) + "): Connected...");
		connected = true;
	}
}

void Client::run()
{
	serverEndpoint = *UDPResolver(context).resolve(asio::ip::udp::v4(), host, toString(port)).begin();
	logDebug(serverEndpoint.address().to_string() + ":" + toString(serverEndpoint.port()));
	socket.open(asio::ip::udp::v4());
	lastReceived = now();
	while (keepRunning)
	{
		if (!connected) connectToServer();

		if (socket.available())
		{
			Array<Char, sizeof(Message)> inMessageBuffer;

			socket.receive_from(asio::buffer(inMessageBuffer, sizeof(Message)), serverEndpoint);
			inMessage = *reinterpret_cast<Message*>(&inMessageBuffer);

			switch (inMessage.event.serverEvent)
			{
			case ServerToClientMessages::DeclineConnection: connectionDeclined();
				break;
			case ServerToClientMessages::ConfirmConnection: connectionConfirmed();
				break;
			case ServerToClientMessages::BroadcastPlayerData: playerDataReceived();
				break;
			case ServerToClientMessages::BroadcastDisconnectClient: playerDisconnected();
				break;
			case ServerToClientMessages::CheckIsAlive: respondToPing();
				break;
			}
		}
	}

	disconnectFromServer();
}

void Client::waitTillConnected() const
{
	while (!connected) {}
}

void Client::sendMessage()
{
	outMessage.timeStamp                          = now();
	lastSent                                      = now();
	Array<Char, sizeof(Message)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(Message)>*>(&outMessage);
	socket.send_to(asio::buffer(outMessageBuffer), serverEndpoint);
}

void Client::disconnectFromServer()
{
	logDebug("Client (" + toString(clientID) + "): Disconnecting...");
	outMessage.event.clientEvent = ClientToServerMessages::WantsToDisconnect;
	outMessage.clientID          = clientID;
	sendMessage();
	connected = false;
}

void Client::connectToServer()
{
	logDebug("Client (" + toString(clientID) + "): Client trying to connect...");
	outMessage.event.clientEvent = ClientToServerMessages::WantsToConnect;
	outMessage.clientID          = 0;
	sendMessage();
	Sleep(1000);
}

void Client::clientThread(Client& client)
{
	try
	{
		client.run();
	}
	catch (Exception& e)
	{
		logError(e.what());
	}
}
