#include "Networking.hpp"
#include "Debug.hpp"
#include "ASIONetworking.hpp"

// 0 - register new client - receive clientID
// 1 - disconnect clients - disconnect from server
// 2 - keep alive message - keep alive message
// 3 - send message to client - send message to server
// 4 - broadcast message - send to all clients

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
			//logDebug("Server: Event " + toString(inMessage.eventID) + " sent from " + toString(inMessage.clientID));
			switch (inMessage.eventID)
			{
			case 0: registerNewClient();
				break;
			case 1: disconnectClient();
				break;
			case 2: clientIsAlive();
				break;
			default: break;
			}
		}

		//for(ClientInfo& client : clients)
		//{
		//	TimePoint tp = now();
		//	if (tp - client.lastSent > Milliseconds(500))
		//	{
		//		
		//	}

		//}
	}
}

void Server::sendMessage()
{
	Array<Char, sizeof(Message)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(Message)>*>(&outMessage);
	socket.send_to(asio::buffer(outMessageBuffer), senderEndpoint);
}

void Server::waitTillReady() const
{
	while (!running) {}
}

void Server::registerNewClient()
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
				outMessage.eventID  = 0;
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
	outMessage.eventID  = 0;
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

void Server::clientIsAlive()
{
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
	logDebug("Server: Shutting down...");
	keepRunning = false;
	thread.join();
	logDebug("Server: Shut down complete");
}

void Client::start()
{
	logDebug("Client: Starting...");
	thread = Thread(clientThread, std::ref(*this));
}

void Client::stop()
{
	logDebug("Client (" + toString(clientID) + "): Shutting down...");
	keepRunning = false;
	thread.join();
	logDebug("Client (" + toString(clientID) + "): Shut down complete");
}

void Client::run()
{
	serverEndpoint = *UDPResolver(context).resolve(asio::ip::udp::v4(), host, toString(port)).begin();
	socket.open(asio::ip::udp::v4());

	while (keepRunning)
	{
		if (!connected) connectToServer();

		if (socket.available())
		{
			Array<Char, sizeof(Message)> inMessageBuffer;

			socket.receive_from(asio::buffer(inMessageBuffer, sizeof(Message)), serverEndpoint);
			inMessage = *reinterpret_cast<Message*>(&inMessageBuffer);
			switch (inMessage.eventID)
			{
			case 0: changeClientID(inMessage.clientID);
				break;
			case 1: disconnectFromServer();
				break;
			default: customEvents(inMessage);
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
	Array<Char, sizeof(Message)> outMessageBuffer = *reinterpret_cast<Array<Char, sizeof(Message)>*>(&outMessage);
	socket.send_to(asio::buffer(outMessageBuffer), serverEndpoint);
}

void Client::changeClientID(UShort clientID)
{
	logDebug("Client (" + toString(clientID) + "): Server requested ID change");
	this->clientID = clientID;
	logDebug("Client (" + toString(clientID) + "): Client was assigned ID:" + toString(clientID));

	if (!connected)
	{
		logDebug("Client (" + toString(clientID) + "): Connected...");
		connected = true;
	}
}

void Client::disconnectFromServer()
{
	logDebug("Client (" + toString(clientID) + "): Disconnecting...");
	outMessage.eventID  = 1;
	outMessage.clientID = clientID;
	sendMessage();
	connected = false;
}

void Client::connectToServer()
{
	logDebug("Client (" + toString(clientID) + "): Client trying to connect...");
	outMessage.eventID  = 0;
	outMessage.clientID = 0;
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

void Client::customEvents(Message& message) {}
