#pragma once

#include "ASIONetworking.hpp"
#include "Message.hpp"
#include "Settings.hpp"

void readMessage(NetworkingTcpSocket& socket, Message& message);

void writeMessage(asio::ip::tcp::socket& socket, Message& message);

void handleRead(const Bool& keepRunning, asio::ip::tcp::socket& socket);

void handleWrite(const Bool& keepRunning, asio::ip::tcp::socket& socket);

struct Client
{
	void run() const;
};
