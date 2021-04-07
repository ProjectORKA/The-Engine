
#include "Networking.hpp"

Server server;
Client client;

void Server::create()
{
}

void Server::connect()
{
	socket.connect(endpoint);
}

void Server::send()
{
	Message message;
	message.messageID = 1;
	message.bodyByteSize = 12;
	message.body[0] = 'H';
	message.body[1] = 'e';
	message.body[2] = 'l';
	message.body[3] = 'l';
	message.body[4] = 'o';
	message.body[5] = '_';
	message.body[6] = 'W';
	message.body[7] = 'o';
	message.body[8] = 'r';
	message.body[9] = 'l';
	message.body[10] = 'd';
	message.body[11] = '!';

	ASIOBuffer buffer(&message, message.bodyByteSize + 8);

	if (socket.is_open()) {
		socket.send(buffer);
	}
}